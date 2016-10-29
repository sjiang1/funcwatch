#include <stdio.h>
#include <debug_printf.h>
#include "funcwatch.h"
#include "dwarf_util.h"

static Dwarf_Unsigned get_size_from_type_die(Dwarf_Die type_die){
  Dwarf_Unsigned var_size = 0;
  Dwarf_Attribute attr;
  Dwarf_Half tag;
  Dwarf_Error err;
  int rc = DW_DLV_OK;
  
  Dwarf_Die current_type_die = type_die;
  
  rc = dwarf_attr(current_type_die,DW_AT_byte_size,&attr,&err);
  if(rc == DW_DLV_OK)
    rc = dwarf_formudata(attr, &var_size, &err);
  else if(rc == DW_DLV_NO_ENTRY){
    rc = dwarf_attr(type_die,DW_AT_bit_size,&attr,&err);
    if(rc == DW_DLV_OK){
      rc = dwarf_formudata(attr, &var_size, &err);
      var_size = (var_size+7)/8;
    }
  }
  return var_size;
}

static char *get_name_from_type_die(Dwarf_Die type_die){
  Dwarf_Error err;
  char *type_name = NULL;
  Dwarf_Half tag;
  int rc = DW_DLV_OK;

  rc = dwarf_tag(type_die, &tag, &err);
  if(rc != DW_DLV_OK) {
    debug_printf("Error: %s\n", dwarf_errmsg(err));
    return;
  }
  
  if(tag == DW_TAG_typedef){
    rc = dwarf_diename(type_die, &type_name, &err);
    if(rc == -1){
      debug_printf("Error, cannot get type name from %s\n", "type def");
      type_name = NULL;
    }
  }

  if( tag == DW_TAG_pointer_type || tag == DW_TAG_array_type ){
    //note that dwarf classifies pointers as "symbol"
    // so we need to add the * manually later when printing

    type_name = malloc(sizeof(char)*2);
    type_name[0] = '*';
    type_name[1] = '\0';
  }
  if( tag == DW_TAG_subroutine_type ) {
    type_name = malloc(sizeof(char) * 17);
    strcpy(type_name, "function pointer");
    type_name[16] = '\0';
    return;
  }
  else if(type_name ==NULL){
    rc = dwarf_diename(type_die, &type_name, &err);
    if(rc != DW_DLV_OK ){
      debug_printf("Warning: cannot get type name from %s\n", "a type die");
      type_name = NULL;
    }
  }
  
  return type_name;
}

static void assign_type_flags(funcwatch_param *p, Dwarf_Die type_die){
  Dwarf_Error err;
  Dwarf_Half tag;
  int rc = dwarf_tag(type_die, &tag, &err);
  if (rc != DW_DLV_OK) {
    debug_printf("Error: %s\n", dwarf_errmsg(err));
    p->flags |= FW_INVALID;
    return;
  }

  if (tag == DW_TAG_pointer_type){
    p->flags |= FW_POINTER;
  }
  else if (tag == DW_TAG_array_type){
    p->flags |= FW_POINTER;
    p->flags |= FW_ARRAY;
  }
  else if (tag == DW_TAG_structure_type ) {
    p->flags |= FW_STRUCT;
  }
  else if(tag == DW_TAG_enumeration_type) {
    p->flags |= FW_ENUM;
  }
  else if(tag == DW_TAG_union_type){
    p->flags |= FW_UNION;
  }
  else if(tag == DW_TAG_base_type) {
    rc = 0;
    Dwarf_Unsigned encoding;
    Dwarf_Attribute attr;
    rc = dwarf_attr(type_die,DW_AT_encoding,&attr,&err);
    rc = dwarf_formudata(attr, &encoding, &err);
    switch(encoding) {
    case DW_ATE_unsigned:
      p->flags |= FW_INT;
      break;
    case DW_ATE_signed:
      p->flags |= FW_INT|FW_SIGNED;
      break;
    case DW_ATE_unsigned_char:
      p->flags |= FW_CHAR;
      break;
    case DW_ATE_signed_char:
      p->flags |= FW_CHAR|FW_SIGNED;
      break;
    case DW_ATE_float:
      p->flags |= FW_FLOAT;
    default:
      break;
    }
  }
  else if(tag == DW_TAG_subroutine_type) {
    p->flags |= FW_POINTER | FW_VOID | FW_INVALID;
  }  
  else if(tag == DW_TAG_const_type){
    p->flags |= FW_CONSTANT;
  }
}

static void get_type_info_from_type_die(Dwarf_Debug dbg, Dwarf_Die type_die, funcwatch_param *p){
  Dwarf_Half tag;
  Dwarf_Error err;
  Dwarf_Attribute attr, attr2;
  int rc = 0;
  Dwarf_Off offset;

  // for typedef, we use type def names for the type.
  p->type = get_name_from_type_die(type_die);

  // the other attributes use the actual types' information
  Dwarf_Die current_type_die = type_die;
  rc = dwarf_tag(current_type_die, &tag, &err);
  while(tag == DW_TAG_typedef) {
    p->flags |= FW_TYPEDEF;
    
    rc = dwarf_attr(type_die,DW_AT_type,&attr,&err);
    if( rc != DW_DLV_OK){
      debug_printf("Error: cannot get type defined by a %s\n", "typedef");
      current_type_die = NULL;
      break;
    }

    Dwarf_Off offset;
    rc = dwarf_global_formref(attr, &offset, &err);

    Dwarf_Die type_die;
    rc = dwarf_offdie_b(dbg, offset, 1, &current_type_die, &err);
    
    /* rc = dwarf_formudata(attr, &current_type_die, &err);
    if( rc != DW_DLV_OK){
      debug_printf("Error: cannot get tag from a %s\n", "type die");
      current_type_die = NULL;
      break;
      }*/
    rc = dwarf_tag(current_type_die, &tag, &err);
  }

  p->type_die = current_type_die;
  p->size = get_size_from_type_die(current_type_die);
  
  assign_type_flags(p, current_type_die);
}

void get_type_info_from_parent_type_die(Dwarf_Debug dbg, Dwarf_Die parent_type_die, funcwatch_param *p){
  Dwarf_Error err;
  Dwarf_Attribute attr;
  int rc = 0;

  rc = dwarf_attr(parent_type_die, DW_AT_type, &attr, &err);
  
  // void * is represented as just a pointer entry with no underlying type
  if (rc == DW_DLV_NO_ENTRY) {
    p->flags |= FW_VOID;
    p->type = "void";
    return;
  }
  else if (rc != DW_DLV_OK) {
    debug_printf("Error: %s\n", dwarf_errmsg(err));
    p->flags |= FW_INVALID;
    return;
  }
  
  Dwarf_Off offset;
  rc = dwarf_global_formref(attr, &offset, &err);

  Dwarf_Die type_die;
  rc = dwarf_offdie_b(dbg, offset, 1, &type_die, &err);
  get_type_info_from_type_die(dbg, type_die, p);
}

void get_type_info_from_var_die(Dwarf_Debug dbg, Dwarf_Die var_die, funcwatch_param *p){
  p->flags = 0;
  int rc;
  Dwarf_Error err;
  Dwarf_Attribute attr;
  Dwarf_Die type_die;
  Dwarf_Off offset;
  rc = dwarf_attr(var_die,DW_AT_type,&attr, &err);
  if (rc != DW_DLV_OK) {
    debug_printf("Error: %s\n", dwarf_errmsg(err));
    exit(-1);;
  }
  rc =dwarf_global_formref(attr,&offset, &err);
  if (rc != DW_DLV_OK) {
    debug_printf("Error: %s\n", dwarf_errmsg(err));
    exit(-1);
  }

  rc = dwarf_offdie_b(dbg,offset,1,&type_die,&err);
  if (rc != DW_DLV_OK) {
    debug_printf("Error: %s\n", dwarf_errmsg(err));
    exit(-1);;
  }

  get_type_info_from_type_die(dbg, type_die, p);
}

