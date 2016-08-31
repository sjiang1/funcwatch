#include <sys/types.h>
#include <stdint.h>
#include <udis86.h>
#include <debug_printf.h>

void set_breakpoints(uint8_t *old_buf, uint8_t *new_buf, size_t sz) {
  new_buf[0] = 0xcc;
  ud_t ud;
  ud_init(&ud);
  ud_set_mode(&ud, 32);
  ud_set_pc(&ud, 0);
  ud_set_input_buffer(&ud, old_buf, sz);
  ud_set_syntax(&ud, UD_SYN_INTEL);

  /* the following is an example if we want to set the breakpoints after some instructions.
  uint64_t first_breakpoint_offset = ud_disassemble(&ud);
  const char *instr1 = ud_insn_asm(&ud);
  debug_printf("Instr: %s\t%d\n", instr1, first_breakpoint_offset);
  
  first_breakpoint_offset += ud_disassemble(&ud);
  const char *instr2 = ud_insn_asm(&ud);
  debug_printf("Instr: %s\t%d\n", instr2,  first_breakpoint_offset);
  
  first_breakpoint_offset += ud_disassemble(&ud);
  const char *instr3 = ud_insn_asm(&ud);
  debug_printf("Instr: %s\t%d\n", instr3,  first_breakpoint_offset);
  
  first_breakpoint_offset += ud_disassemble(&ud);
  const char *instr4 = ud_insn_asm(&ud);
  debug_printf("Instr: %s\t%d\n", instr4,  first_breakpoint_offset);
  
  first_breakpoint_offset += ud_disassemble(&ud);
  const char *instr5 = ud_insn_asm(&ud);
  debug_printf("Instr: %s\t%d\n", instr5,  first_breakpoint_offset);

  // first_breakpoint_offset = ud_insn_off(&ud);
  debug_printf("Setting break point at offset %llu\n", first_breakpoint_offset );
  new_buf[ first_breakpoint_offset ] = 0xcc;
  */
  while(ud_disassemble(&ud)) {
    // const char *instr = ud_insn_asm(&ud);
    // debug_printf("Instr: %s\n", instr);
    if(ud_insn_mnemonic(&ud) == UD_Iret) {
      uint64_t offset = ud_insn_off(&ud);
      debug_printf("Setting break point at offset %llu\n", offset);
      new_buf[offset] = 0xcc;
    }
  }
}
