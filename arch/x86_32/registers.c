#include <stdlib.h>
#include <stdint.h>
#include <dwarf.h>
#include <libdwarf.h>
#include <sys/user.h>
#include <debug_printf.h>


void rewind_breakpoint(struct user_regs_struct *r) {
	r->eip --;
}
uint64_t get_instruction_pointer(struct user_regs_struct *r) {
	return r->eip;
}
Dwarf_Unsigned get_register_by_number(struct user_regs_struct * registers, Dwarf_Half id) {

	switch(id) {
	case 0:
		return registers->eax;
	case 1:
		return registers->ecx;
	case 2:
		return registers->edx;
	case 3:
		return registers->ebx; // why? No idea.
	case 4:
		return registers->esp;
	case 5:
		return registers->ebp;
	case 6:
		return registers->esi;
	case  7:
		return registers->edi;
	case 8:
		return registers->eip;
	case 9:
		return registers->eflags;
	case 10:
		return registers->xcs;
	case 11:
		return registers->xss;
	case 12:
		return registers->xds;
	case 13:
return registers->xes;
	case 14:
		return registers->xfs;
	case 15:
		return registers->xgs;

	default: // unsupported register id. Die because there's no other guaranteed way to show failure
		debug_printf("Invalid register id:%d\n", id);
		exit(-1);

	}
	return 0;
}

unsigned long get_return_register(struct user_regs_struct *registers) {
	return registers->eax;
}


uint64_t get_frame_base(struct user_regs_struct *registers) {
	return registers->esp+4;
}

unsigned long get_return_register_float(struct user_fpregs_struct * registers) {
	unsigned long *l;
	l = (unsigned long *) registers->st_space;
	return *l;
}
