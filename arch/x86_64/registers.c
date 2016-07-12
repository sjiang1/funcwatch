#include <stdint.h>
#include <stdlib.h>
#include <dwarf.h>
#include <libdwarf.h>
#include <sys/user.h>
#include <debug_printf.h>


void rewind_breakpoint(struct user_regs_struct *r) {
	r->rip --;
}
uint64_t get_instruction_pointer(struct user_regs_struct *r) {
	return r->rip;
}
Dwarf_Unsigned get_register_by_number(struct user_regs_struct * registers, Dwarf_Half id) {

	switch(id) {
	case 0:
		return registers->rax;
	case 1:
		return registers->rdx;
	case 2:
		return registers->rcx;
	case 3:
		return registers->rbx; // why? No idea.
	case 4:
		return registers->rsi;
	case 5:
		return registers->rdi;
	case 6:
		return registers->rbp;
	case  7:
		return registers->rsp;
	case 8:
		return registers->r8;
	case 9:
		return registers->r9;
	case 10:
		return registers->r10;
	case 11:
		return registers->r11;
	case 12:
		return registers->r12;
	case 13:
		return registers->r13;
	case 14:
		return registers->r14;
	case 15:
		return registers->r15;
	case 16:
		return registers->rip;
	default: // unsupported register id. Die because there's no other guaranteed way to show failure
		debug_printf("Invalid register id:%d\n", id);
		exit(-1);

	}
	return 0;
}

uint64_t get_return_register(struct user_regs_struct *registers) {
	return registers->rax;
}

uint64_t get_frame_base(struct user_regs_struct *registers) {
	return registers->rsp+8;
}

unsigned long get_return_register_float(struct user_fpregs_struct * registers) {
	unsigned long *l;
	l = (unsigned long *) registers->st_space;
	return *l;
}
