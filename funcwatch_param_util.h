#ifndef _FUNCWATCH_PARAM_UTIL_
#define _FUNCWATCH_PARAM_UTIL_

#include <stdio.h>
#include "funcwatch.h"

funcwatch_param *getFirstNonPointer(funcwatch_param *pointer_param);
funcwatch_param *get_end_of_list(funcwatch_param *param);
void funcwatch_param_initialize(funcwatch_param *param);

#endif // _FUNCWATCH_PARAM_UTIL_
