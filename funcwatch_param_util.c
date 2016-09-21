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

void funcwatch_param_initialize(funcwatch_param *param){
  param->name = NULL;
  param->func_name = NULL;
  param->call_num = -1;

  param->type_die = NULL;
  param->var_die = NULL;

  param->type = NULL;
  param->size = 0;
  param->flags = 0;

  param->addr = NULL;
  param->value = NULL;
  param->value_float = 0;
  
  param->next = NULL;
  param->struct_level = 0;
  
}
