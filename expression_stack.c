/*
 * Internal stuff for solving dwarf expressions to find addresses of function arguments and local variables
 */
#include <stdlib.h>
#include <debug_printf.h>
#include <sys/ptrace.h>
#include <errno.h>

#include "funcwatch.h"
#include "arch.h"
expression_stack *create_stack() {
  expression_stack *stack = (expression_stack *) malloc(sizeof(expression_stack));
  stack->length=0;
  stack->max=255;
  stack->base= (Dwarf_Unsigned *) malloc(stack->max*sizeof(Dwarf_Unsigned));
  return stack;
}

void push(expression_stack *stack, Dwarf_Unsigned p) {
  if(stack->length == stack->max) {
    stack->max*=2;
    stack->base = (Dwarf_Unsigned *) realloc(stack->base, stack->max*sizeof(Dwarf_Unsigned));
  }
  stack->base[stack->length] = p;
  ++stack->length;
}

Dwarf_Unsigned pop(expression_stack *stack) {
  if(stack->length > 0) {
    --stack->length;
    return stack->base[stack->length];
  }
  else
    return 0;
}

void free_stack(expression_stack *stack) {
  free(stack->base);
  free(stack);
}

int size_stack(expression_stack *stack){
  return stack->length;
}

//parses a location expression. The result is the top entry on the stack as per the Dwarf spec.
void parse_location(funcwatch_run *run, Dwarf_Loc* loc, expression_stack *stack, expression_stack *flags, user_regs_struct *registers, Dwarf_Unsigned fbreg) {
  int flag = 0;
  //we assume DW_OP_LIT[0-31] are sequential
  if(loc->lr_atom >= DW_OP_lit0&& loc->lr_atom <= DW_OP_lit31) {
    push(stack, loc->lr_atom-DW_OP_lit0);
    push(flags, flag);
  }
  else if(loc->lr_atom >= DW_OP_reg0&& loc->lr_atom <= DW_OP_reg31) {
    // the actual variable value we want is in a register
    push(stack, get_register_by_number(registers, loc->lr_atom-DW_OP_reg0));
    flag |= FW_REG_VALUE;
    push(flags, flag);
  }
  else if(loc->lr_atom >= DW_OP_breg0&& loc->lr_atom <= DW_OP_breg31) {
    // the actual variable value we want is lr_atom + the value in a register
    push(stack, get_register_by_number(registers, loc->lr_atom-DW_OP_breg0)+loc->lr_number);
    flag |= FW_REG_VALUE;
    push(flags, flag);
  }
  else {
    switch(loc->lr_atom) {
    case DW_OP_call_frame_cfa:
      push(stack, call_frame_cfa(run, registers)+sizeof(void *));
      push(flags, flag);
      break;
    case DW_OP_dup: {
      Dwarf_Unsigned d = pop(stack);
      push(stack, d);
      push(stack, d);
      Dwarf_Unsigned f = pop(flags);
      push(flags, f);
      push(flags, f);
      break;
    }
    case DW_OP_pick: {
      push(stack, stack->base[(stack->length)-1-(loc->lr_number)]);
      push(flags, flag);
      break;
    }
    case DW_OP_over: {
      push(stack, stack->base[stack->length-2]);
      push(flags, flag);
      break;
    }
    case DW_OP_swap: {
      Dwarf_Unsigned d1 = pop(stack);
      Dwarf_Unsigned d2 = pop(stack);
      push(stack, d1);
      push(stack, d2);
      Dwarf_Unsigned f1 = pop(flags);
      Dwarf_Unsigned f2 = pop(flags);
      push(flags, f1);
      push(flags, f2);
      break;
    }
    case DW_OP_rot: {
      Dwarf_Unsigned d1 = pop(stack);
      Dwarf_Unsigned d2 = pop(stack);
      Dwarf_Unsigned d3 = pop(stack);
      push(stack, d1);
      push(stack, d3);
      push(stack, d2);
      Dwarf_Unsigned f1 = pop(flags);
      Dwarf_Unsigned f2 = pop(flags);
      Dwarf_Unsigned f3 = pop(flags);
      push(flags, f1);
      push(flags, f3);
      push(flags, f2);

      break;
    }
      // begin arithmetic operations
    case DW_OP_abs: {
      Dwarf_Signed d = (Dwarf_Signed) pop(stack);
      if(d < 0)
	push(stack, d*-1);
      else
	push(stack, d);

      pop(flags);
      push(flags, flag);
      break;
    }
    case DW_OP_and: {
      Dwarf_Unsigned d1 = pop(stack);
      Dwarf_Unsigned d2 = pop(stack);
      push(stack, d1&d2);
      pop(flags);
      pop(flags);
      push(flags, flag);
      break;
    }
    case DW_OP_div: {
      Dwarf_Signed bottom = (Dwarf_Signed) pop(stack);
      Dwarf_Signed top= (Dwarf_Signed) pop(stack);
      push(stack, top/bottom);
      break;
    }
    case DW_OP_minus: {
      Dwarf_Unsigned d1 = pop(stack);
      Dwarf_Unsigned d2 = pop(stack);
      push(stack, d2-d1);
      pop(flags);
      pop(flags);
      push(flags, flag);
      break;
    }
    case DW_OP_mod: {
      Dwarf_Unsigned d1 = pop(stack);
      Dwarf_Unsigned d2 = pop(stack);
      push(stack, d2%d1);
      pop(flags);
      pop(flags);
      push(flags, flag);
      break;
    }
    case DW_OP_mul: {
      Dwarf_Unsigned d1 = pop(stack);
      Dwarf_Unsigned d2 = pop(stack);
      push(stack, d2*d1);
      pop(flags);
      pop(flags);
      push(flags, flag);
      break;
    }
    case DW_OP_neg: {
      Dwarf_Signed d = (Dwarf_Signed) pop(stack);
      push(stack, d*-1);
      pop(flags);
      push(flags, flag);
      break;
    }
    case DW_OP_not: {
      Dwarf_Unsigned d = pop(stack);
      push(stack, !d);
      pop(flags);
      push(flags, flag);
      break;
    }
    case DW_OP_or: {
      Dwarf_Unsigned d1 = pop(stack);
      Dwarf_Unsigned d2 = pop(stack);
      push(stack, d2|d1);
      pop(flags);
      pop(flags);
      push(flags, flag);
      break;
    }
    case DW_OP_plus: {
      Dwarf_Unsigned d1 = pop(stack);
      Dwarf_Unsigned d2 = pop(stack);
      push(stack, d2+d1);
      pop(flags);
      pop(flags);
      push(flags, flag);
      break;
    }
    case DW_OP_shl: {
      Dwarf_Unsigned d1 = pop(stack);
      Dwarf_Unsigned d2 = pop(stack);
      push(stack, d2<<d1);
      pop(flags);
      pop(flags);
      push(flags, flag);
      break;
    }
    case DW_OP_shr: {
      Dwarf_Unsigned d1 = pop(stack);
      Dwarf_Unsigned d2 = pop(stack);
      push(stack, d2>>d1);
      pop(flags);
      pop(flags);
      push(flags, flag);
      break;
    }
    case DW_OP_xor: {
      Dwarf_Unsigned d1 = pop(stack);
      Dwarf_Unsigned d2 = pop(stack);
      push(stack, d2^d1);
      pop(flags);
      pop(flags);
      push(flags, flag);
      break;
    }
    case DW_OP_fbreg: {
      push(stack, fbreg+ (Dwarf_Signed) loc->lr_number);
      push(flags, flag);
      break;
    }
    default: // unsupported operation. fail.
      debug_printf("Unsupported dwarf opcode 0x%x\n",loc->lr_atom);
      flag |= FW_INVALID;
      push(flags, flag);
      break;
    }
  }
}

/*
 Evalute the address of a die, depending on the given tag.
 Possible tag input may be: DW_AT_segment, DW_AT_return_addr, DW_AT_frame_base, DW_AT_static_link,
  DW_AT_data_member_location, DW_AT_string_length, DW_AT_location, DW_AT_use_location,
  DW_AT_vtable_elem_location,
 The tags that are known to be used in Funcwatch: DW_AT_location and DW_AT_data_member_location
 Return DW_DLV_OK if the address is retrived correctly.
 */
int evaluate_address(funcwatch_run *run, Dwarf_Die *die, Dwarf_Half tag,
		     Dwarf_Unsigned fbreg, int *flags, Dwarf_Unsigned *ret_address) {
  user_regs_struct *registers = (user_regs_struct *) malloc(sizeof(user_regs_struct));
  memset(registers, 0, sizeof(user_regs_struct));
  long rc = ptrace(PTRACE_GETREGS, run->child_pid, 0, registers);
  if(rc != 0) {
    debug_printf("Unable to get registers for setting function parameter in pid: %d: %s\n", run->child_pid, strerror(errno));
    exit(-1);
  }
  
  Dwarf_Error err;
  Dwarf_Attribute attr;
  Dwarf_Half var_tag;
  if (dwarf_tag(*die, &var_tag, &err) != DW_DLV_OK)
    return;

  int ares = dwarf_attr(*die, tag, &attr, &err);
  if( ares != DW_DLV_OK) {
    if(ares == DW_DLV_ERROR)
      debug_printf("Error in getting attr: %s\n", dwarf_errmsg(&err));
    else
      debug_printf("Error: Do not have location attribute.%s\n", " ");
    return ares;
  }
  
  Dwarf_Signed len;
  Dwarf_Locdesc **lbuf;
  expression_stack *stack = create_stack();
  int lres = dwarf_loclist_n(attr, &lbuf, &len, &err);
  if ( lres != DW_DLV_OK) {
    debug_printf("Error in getting loclist: %s\n", dwarf_errmsg(&err));
    return lres;
  }
  Dwarf_Locdesc *locdescs = *lbuf;
  expression_stack *internal_flags = create_stack();
  
  for(int i = 0; i < len; ++i) {
    for(int j = 0; j < lbuf[i]->ld_cents; ++j) {
      Dwarf_Loc loc = lbuf[i]->ld_s[j];
      parse_location(run, &loc, stack, internal_flags, registers, fbreg);
    }
    //deallocate stuff piece by piece
    //TODO: figure out why this started failing.
    dwarf_dealloc(run->dwarf_ptr, lbuf[i]->ld_s, DW_DLA_LOC_BLOCK);
    dwarf_dealloc(run->dwarf_ptr,lbuf[i], DW_DLA_LOCDESC);
  }
  dwarf_dealloc(run->dwarf_ptr, lbuf, DW_DLA_LIST);

  /*Dwarf_Attribute const_attr;
  ares = dwarf_attr(*die, DW_AT_const_value, &const_attr, &err);
  if(ares != DW_DLV_OK) {
    if(ares == DW_DLV_ERROR){
      debug_printf("Error in getting attr: %s\n", dwarf_errmsg(&err));
      return ares;
    }
    else if(ares == DW_DLV_NO_ENTRY){      
      *ret_address = pop(stack);
      *flags = pop(internal_flags);
      free_stack(stack);
      free_stack(internal_flags);
      //debug_printf("Returning %x\n", ret);
      return DW_DLV_OK;
    }
    else
      debug_printf("Error: Do not have location attribute.%s\n", " ");
    return ares;
  }
  */
  int flag = pop(internal_flags);
  Dwarf_Unsigned address = pop(stack);
  
  int visited_address_number = 1;
  int total_number = size_stack(stack);
  while(flag & FW_REG_VALUE && !(flag & FW_INVALID)
	&& visited_address_number < total_number){
    visited_address_number ++;
    flag = pop(internal_flags);
    address = pop(stack);
  }
  *ret_address = address;
  *flags = flag;
  free_stack(stack);
  free_stack(internal_flags);
  //debug_printf("Returning %x\n", ret);
  return DW_DLV_OK;
}


Dwarf_Unsigned call_frame_cfa(funcwatch_run *run, user_regs_struct *registers) {

  Dwarf_Cie *cie_data; 
  Dwarf_Signed cie_count; 
  Dwarf_Fde *fde_data; 
  Dwarf_Signed fde_count;
  Dwarf_Error err; 
  int rc; 
  rc = dwarf_get_fde_list(run->dwarf_ptr,&cie_data,&cie_count, &fde_data,&fde_count,&err);
  if (rc == DW_DLV_NO_ENTRY)
    rc = dwarf_get_fde_list_eh(run->dwarf_ptr,&cie_data,&cie_count, &fde_data,&fde_count,&err);  
  if (rc != DW_DLV_OK) {
    debug_printf("Error: %s\n", dwarf_errmsg(err));
    exit(-1);
  }
  Dwarf_Fde fde;
  Dwarf_Addr lowpc, highpc;

  /*
   * If eip is not at the (begining of the function - 1), i.e.,the eip is not at the caller site
   * we need to adjust get_instruction_pointer(registers) to get_instruction_pointer(registers) - offset
   * the offset can be obtained by set_breakpoints function
   */
  rc = dwarf_get_fde_at_pc(fde_data, get_instruction_pointer(registers), &fde, &lowpc, &highpc, &err);
  if (rc != DW_DLV_OK) {
    debug_printf("Error: %s\n", dwarf_errmsg(err));
    exit(-1);
  }
  Dwarf_Small value_type;
  Dwarf_Signed offset_relevant, register_num, offset_or_block_len;
  Dwarf_Ptr  block_ptr;
  Dwarf_Addr row_pc_out;
  
  /*
   * If eip is not at the (begining of the function - 1), i.e., the eip is not at the caller site
   * we need to adjust get_instruction_pointer(registers) to get_instruction_pointer(registers) - offset
   * the offset can be obtained by set_breakpoints function
   */
  rc = dwarf_get_fde_info_for_cfa_reg3(fde, get_instruction_pointer(registers), &value_type, &offset_relevant, &register_num, &offset_or_block_len, &block_ptr, &row_pc_out, &err);
  if (rc != DW_DLV_OK) {
    debug_printf("Error: %s\n", dwarf_errmsg(err));
    exit(-1);
  }
  Dwarf_Unsigned cfa;

  /*
   * If eip is not at the (begining of the function - 1), i.e., the eip is not at the caller site
   * we need to rewind the register's value, according to set_breakpoints function
   * That is, we need to get the register's value before the function gets called
   */
  if(value_type == DW_EXPR_OFFSET)
    cfa =  get_register_by_number(registers, register_num);
  else {
    debug_printf("Unsupported CFA value type: %x. Terminating.\n", value_type);
    exit(-1);
  }
  dwarf_fde_cie_list_dealloc(run->dwarf_ptr, cie_data, cie_count, 
			     fde_data,fde_count); 
  return cfa;
}
