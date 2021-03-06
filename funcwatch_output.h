#if __cplusplus
extern "C" {
#endif

#ifndef _FUNCWATCH_OUTPUT_
#define _FUNCWATCH_OUTPUT_

#include <stdio.h>
#include "funcwatch.h"
#include "vector.h"
#include "dynstring.h"

  
  /*--the function called in main--*/
  void output_logged_values(FILE *f, funcwatch_run *run);

  
  /*--inner functions, declared here for unit testing--*/
  DynString output_logged_values_inner(funcwatch_run *run);
  DynString print_param_list(funcwatch_param *p, int is_return);
  // print_param_vector is used only for **params**
  // (which can be params when before func calls, and after func calls)
  DynString print_param_vector(Vector params, int is_return_param);
  // the lowest level function for print parameter/return
  DynString print_param(funcwatch_param *p, int is_return);
  
#endif // _FUNCWATCH_OUTPUT_
#if __cplusplus
}
#endif
