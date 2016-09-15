#include <stdio.h>
#include <debug_printf.h>
#include "funcwatch.h"
#include "dwarf_util.h"

static void get_type_info_from_type_die(Dwarf_Debug dbg, Dwarf_Die type_die, funcwatch_param *p){
  Dwarf_Half tag;
  Dwarf_Error err;
  Dwarf_Attribute attr, attr2;
  int rc = 0;
  Dwarf_Unsigned var_size;
  Dwarf_Off offset;
  
  p->type_die = type_die;

  rc = dwarf_tag(type_die, &tag, &err);
  if (rc != DW_DLV_OK) {
    debug_printf("Error: %s\n", dwarf_errmsg(err));
    p->flags |= FW_INVALID;
    return;
  }
  if(tag == DW_TAG_typedef) {
    p->flags |= FW_TYPEDEF;
  }
  else {
    if (tag == DW_TAG_pointer_type
	|| tag == DW_TAG_array_type)
      p->flags |= FW_POINTER;

    rc = dwarf_attr(type_die,DW_AT_byte_size,&attr,&err);
    var_size = 0;
    if(rc == DW_DLV_OK)
      rc = dwarf_formudata(attr, &var_size, &err);
    else if(rc == DW_DLV_NO_ENTRY){
      rc = dwarf_attr(type_die,DW_AT_bit_size,&attr,&err);
      if(rc == DW_DLV_OK){
	rc = dwarf_formudata(attr, &var_size, &err);
	var_size = (var_size+7)/8;
      }
    }
  }

  char *type_name = NULL;
  if(tag == DW_TAG_typedef){
    rc = dwarf_diename(type_die, &type_name, &err);
    if(rc == -1){
      debug_printf("Error, cannot get type name from %s", p->name);
      type_name = NULL;
    }
  }

  while (tag == DW_TAG_pointer_type
	          || tag == DW_TAG_typedef
	          ||tag == DW_TAG_const_type
	 || tag == DW_TAG_array_type) {
    // we ignore function pointers outright
    rc = dwarf_attr(type_die,DW_AT_type,&attr, &err);
    // void * is represented as just a pointer entry with no underlying type
    if (rc == DW_DLV_NO_ENTRY) {
      p->flags |= FW_VOID;
      p->type = "void";
      p->size = var_size;
      return;
    }
    else if (rc != DW_DLV_OK) {
      debug_printf("Error: %s\n", dwarf_errmsg(err));
      p->flags |= FW_INVALID;
      return;
    }
    rc = dwarf_global_formref(attr,&offset, &err);
    rc = dwarf_offdie_b(dbg,offset,1,&type_die,&err);
    p->type_die = type_die;

    rc = dwarf_diename(type_die, &type_name, &err);
    rc = dwarf_tag(type_die, &tag, &err);
    if (rc != DW_DLV_OK) {
      p->flags |= FW_INVALID;
      debug_printf("Error: %s\n", dwarf_errmsg(err));
      return;
    }
    if(tag == DW_TAG_const_type
       || tag == DW_TAG_array_type)
      continue;
    if(tag == DW_TAG_subroutine_type) {
      p->flags = FW_POINTER | FW_VOID | FW_INVALID;
      p->type = "function pointer";
      return;
    }
    if(tag == DW_TAG_typedef) {
      p->flags |= FW_TYPEDEF;
      continue;
    }
    else if(tag == DW_TAG_pointer_type){
      p->flags |= FW_POINTER;
    }
    else if(tag == DW_TAG_structure_type){
      p->flags |= FW_STRUCT;
    }
    else if(tag == DW_TAG_enumeration_type) {
      p->flags |= FW_ENUM;
    }
    // we set size to the size of the actual variable
    //the contents of the pointer will be resolved later
    rc = dwarf_attr(type_die,DW_AT_byte_size,&attr,&err);
    int rc2 = dwarf_attr(type_die,DW_AT_bit_size,&attr2,&err);
    if(rc == DW_DLV_OK)
      rc = dwarf_formudata(attr, &var_size, &err);
    else if(rc == DW_DLV_NO_ENTRY && rc2 == DW_DLV_OK){
      rc = dwarf_formudata(attr2, &var_size, &err);
      var_size = (var_size+7)/8;
    }
    else{
      //TODO: figure out how to dereference incomplete types
      //Till then, we treat them as void *
      rc = dwarf_attr(type_die,DW_AT_declaration,&attr,&err);
      Dwarf_Bool b;
      rc = dwarf_formflag(attr, &b, &err);
      if(b) {
	// for example: AVDictionary ** options;
	rc = dwarf_diename(type_die, &type_name, &err);
	debug_printf("Warning: parameter %s is incomplete, and cannot get a child type from this type die.", type_name);
	p->flags = FW_POINTER | FW_VOID;
	p->type = type_name;
	p->size = var_size;
	return;
      }
      else{
	p->flags |= FW_INVALID;
	p->size = 0;
      }
      // debug_printf("Error getting size of parameter %s: %s\n", p->name, dwarf_errmsg(err));
      // exit(-1);
    }
  }
  
  if(type_name ==NULL){
    rc = dwarf_diename(type_die, &type_name, &err);
    if(rc != DW_DLV_OK ){
      debug_printf("Warning: cannot get type name from %s", p->name);
      type_name = NULL;
    }
  }

  p->size = var_size;
  if(p->size <= 0)
    p->flags |= FW_INVALID;

  p->type = type_name;

  if (tag == DW_TAG_structure_type ) {
    p->flags |= FW_STRUCT;
  }
  else if(tag == DW_TAG_enumeration_type) {
    p->flags |= FW_ENUM;
  }
  else if(tag == DW_TAG_union_type){
    p->flags |= FW_UNION;
  }
  else if (tag == DW_TAG_base_type ) {
    Dwarf_Unsigned encoding;
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
    if(! (p->flags & FW_TYPEDEF))
      p->type = type_name;
    //note that dwarf classifies pointers as "symbol"
    // so we need to add the * manually later when printing
  }
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

