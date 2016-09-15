#ifndef _DWARF_UTIL_
#define _DWARF_UTIL_

#include <stdio.h>
#include "funcwatch.h"

void get_type_info_from_var_die(Dwarf_Debug dbg, Dwarf_Die var_die, funcwatch_param *p) ;

#endif // _DWARF_UTIL_
