#include <stdio.h>
#include "funcwatch.h"
#include "funcwatch_param_util.h"

funcwatch_param *getFirstNonPointer(funcwatch_param *pointer_param){
  funcwatch_param *p = pointer_param;
  
  while((p->flags & FW_POINTER) && p->value != NULL){
    p = p->next;
  }
  
  return p;
}

funcwatch_param *get_end_of_list(funcwatch_param *param){
  while(param->next != NULL)
    param = param->next;
  return param;
}
