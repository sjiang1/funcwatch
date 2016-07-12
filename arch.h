/*
 * Definitions for architecture-specific functions.
 * Implementations can be found under arch/
 *
 */
#include <sys/user.h>
#include <stdint.h>
Dwarf_Unsigned get_register_by_number(struct user_regs_struct * registers, Dwarf_Half id);
void set_breakpoints(uint8_t *old_buf, uint8_t *new_buf, size_t sz);
void rewind_breakpoint(struct user_regs_struct *r);
void set_instruction_pointer(struct user_regs_struct *r, uint64_t i);
uint64_t get_instruction_pointer(struct user_regs_struct *r);
uint64_t get_return_registers(struct user_regs_struct *registers);
uint64_t get_frame_base(struct user_regs_struct *registers);
unsigned long get_return_register(struct user_regs_struct *registers);
unsigned long get_return_register_float(struct user_fpregs_struct * registers);
