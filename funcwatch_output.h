#if __cplusplus
extern "C" {
#endif

#ifndef _FUNCWATCH_OUTPUT_
#define _FUNCWATCH_OUTPUT_

#include <stdio.h>
#include "funcwatch.h"
#include "vector.h"
#include "dynstring.h"

void output_logged_values(FILE *f, funcwatch_run *run);

/*--inner functions, declared here for unit testing--*/
DynString print_param(funcwatch_param *p, int is_return);
DynString print_param_list(funcwatch_param *p);
DynString print_param_vector(Vector params);

funcwatch_param *get_return_of_call_id(funcwatch_param *returns, int returns_cnt, int call_id, int ref);
Vector *get_param_of_call_id(Vector *params, int params_cnt, int call_id, int ref);

#endif // _FUNCWATCH_OUTPUT_
#if __cplusplus
}
#endif
