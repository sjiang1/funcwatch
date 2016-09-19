#ifndef _FUNCWATCH_OUTPUT_
#define _FUNCWATCH_OUTPUT_

#include <stdio.h>
#include "funcwatch.h"

void output_logged_values(FILE *f, funcwatch_run *run);

/*--inner functions, declared here for unit testing--*/
static void print_param(FILE *f, funcwatch_param *p, int is_return);
static void print_param_list(FILE *f, funcwatch_param *p, int is_return);
static void print_param_vector(FILE *f, Vector params);

static funcwatch_param get_return_of_call_id(
					     funcwatch_param *returns, int returns_cnt, int call_id, int ref);
static Vector get_param_of_call_id(
				   Vector *params, int params_cnt, int call_id, int ref);

#endif // _FUNCWATCH_OUTPUT_
