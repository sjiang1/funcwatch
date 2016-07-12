#include <sys/types.h>
#include <stdint.h>
#include <udis86.h>
#include <debug_printf.h>

void set_breakpoints(uint8_t *old_buf, uint8_t *new_buf, size_t sz) {
new_buf[0] = 0xcc;
			    				ud_t ud;
ud_init(&ud);
ud_set_mode(&ud, 64);
ud_set_pc(&ud, 0);
ud_set_input_buffer(&ud, old_buf, sz);
ud_set_syntax(&ud, NULL);
while(ud_disassemble(&ud)) {
if(ud_insn_mnemonic(&ud) == UD_Iret) {
uint64_t offset = ud_insn_off(&ud);
debug_printf("Setting break point at offset %llu\n", offset);
new_buf[offset] = 0xcc;
}
}
}
