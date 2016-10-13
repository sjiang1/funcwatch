#if __cplusplus
extern "C" {
#endif
  
#ifndef _FUNCWATCH_PARAM_UTIL_
#define _FUNCWATCH_PARAM_UTIL_

#include <stdio.h>
#include "funcwatch.h"

  funcwatch_param *getFirstNonPointer(funcwatch_param *pointer_param);
  funcwatch_param *get_end_of_list(funcwatch_param *param);
  void funcwatch_param_initialize(funcwatch_param *param);
  
  // utility function used by output_logged_values_inner, and reevaluate_params
  funcwatch_param *get_return_of_call_id(funcwatch_param *returns, int returns_cnt, int call_id);
  Vector *get_param_of_call_id(Vector *params, int params_cnt, int call_id);
  
#endif // _FUNCWATCH_PARAM_UTIL_
#if __cplusplus
}
#endif
