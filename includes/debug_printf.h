#include <stdio.h>
#define debug_printf(fmt, ...) fprintf(stderr, "%s:%d: " fmt, __FILE__, __LINE__, __VA_ARGS__)
#define debug_printf_error(fmt, ...) fprintf(stderr, "%s:%d:" fmt, __FILE__, __LINE__, __VA_ARGS__);\
	exit(-1)
