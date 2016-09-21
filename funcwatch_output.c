#include "funcwatch.h"
#include "funcwatch_output.h"
#include "vector.h"
#include "dynstring.h"

void output_logged_values(FILE *f, funcwatch_run *run){
  if(DEBUG){
    fprintf(stderr, "Total Call #%d\n", run->num_calls);
    fprintf(stderr, "Total Rets #%d\n", run->num_rets);
  }
  
  fprintf(f, "%s, %s, %s, %s, %s, %s, %s, %s \n",
	  "Is Return Flag", "Function", "Call Number",
	  "Variable Name", "Variable Size",
	  "Usage Flags", "Variable Type", "Value");

  Vector temporary_call_ids;
  vector_init(&temporary_call_ids);
  
  for(int i = 0; i < run->num_calls; ++i)  {
    int curr_call_id = i;
    if(DEBUG)
      fprintf(stderr, "Call #%d\n", i);
    
    int hasParams = 1;
    if(run->params == 0 || run->ret_params == 0){
      fprintf(stderr, "function %s does not have parameters.\n", run->func_name);
      if(run->params != 0 || run->ret_params != 0){
	fprintf(stderr, "Warning: function %s does not have parameters, but have params or ret_params set.\n",
		run->func_name);
      }
      hasParams = 0;
    }
    
    int hasReturn = 1;
    if(run->return_values == NULL) {
      fprintf(stderr, "function %s does not have return value.\n", run->func_name);
      hasReturn = 0;
    }
    
    if(hasParams){
      // print input values
      Vector callparams = run->params[curr_call_id];
      DynString str = print_param_vector(callparams);
    }
    
    if(hasReturn){
      // print return
      funcwatch_param *r = get_return_of_call_id(run->return_values, run->num_rets, curr_call_id, 0);
      DynString str = print_param_list(r);
    }
    
    if(hasParams){
      // print parameters values when function returns
      Vector *ret_p = get_param_of_call_id(run->ret_params, run->num_rets, curr_call_id, 1);
      DynString str = print_param_vector(*ret_p);
    }
  }

  return;
}

static Vector *get_param_of_call_id(Vector *variables, int variables_length, int call_id, int ref){
  for(int i =0; i< variables_length; i++){
    Vector *vector = variables + i;
    if(vector->size > 0){
      funcwatch_param *variable = vector_get(vector, 0);
      if(variable->call_num == call_id){
	return vector;
      }
    }
  }
  return NULL;
}

static funcwatch_param *get_return_of_call_id(funcwatch_param *variables, int variables_length, int call_id, int ref){
  for(int i =0; i< variables_length; i++){
    funcwatch_param *variable = variables + i;
    
    if(variable->call_num == call_id){
      return variable;
    }
  }
  return NULL;
}

static DynString print_param_vector(Vector params){
  if(DEBUG)
    fprintf(stderr, "print param vector\n");

  for(int i =0; i<params.size; i++){
    funcwatch_param *p = vector_get(&params, i);
    DynString paramString = print_param(p, 0);
  }
  DynString str;
  dynstring_init(&str);
  return str;
}

static DynString print_param_list(funcwatch_param *p){
  while(p != NULL) {
    if(DEBUG)
      fprintf(stderr, "print param list, head: %s\n", p->name);
    
    DynString paramString = print_param(p, 1);
    p = p->next;
  }
  DynString str;
  dynstring_init(&str);
  return str;
}

static DynString print_param(funcwatch_param *p, int is_return) {
  char buffer [1024];
  // print is return, has sub-values, function name, call num, parameter name, parameter size, flags
  sprintf(buffer, "%d, %s, %d, %s, %zu,", is_return, p->func_name, p->call_num, p->name, p->size);
  sprintf(buffer, " 0x%x,", p->flags);
  // fprintf(f, "%d, %s, %d, %s, %zu,", is_return, p->func_name, p->call_num, p->name, p->size);
  // fprintf(f, " 0x%x,", p->flags);

  if(p->type == 0) p->type = " ";
  // print parameter type
  if(p->flags & FW_POINTER){
    // fprintf(f, "%s *,", p->type);
    sprintf(buffer, "%s *,", p->type);
  }
  else{
    sprintf(buffer, "%s,", p->type);
  }

  // print parameter value
  if(strcmp(p->type, "unsupported")==0)
    sprintf(buffer, "unsupported value\n");
  else if(p->flags & FW_INVALID)
    sprintf(buffer, "0\n");
  else if(p->flags & FW_POINTER
	  && p->value == 0)
    sprintf(buffer, "%s\n", "NULL");
  else if(p->flags & FW_INT) {
    if(p->flags & FW_SIGNED){
      if(p->flags & FW_POINTER)
	sprintf(buffer, "%ld\n", *(long *)p->value);
      else
	sprintf(buffer, "%ld\n", p->value);
    }
    else{
      if(p->flags & FW_POINTER){
	unsigned long *tmpptr = p->value;
	unsigned long tmpvalue = *tmpptr;
	sprintf(buffer, "%lu\n", tmpvalue);
      }else{
	unsigned long *tmpptr = &(p->value);
	unsigned long tmpvalue = *tmpptr;
	sprintf(buffer, "%lu\n", tmpvalue);
      }
    }
  }else if(p->flags & FW_FLOAT) {
    float *tmpptr = 0;
    if(p->flags & FW_POINTER)
      tmpptr = p->value;
    else
      tmpptr = &(p->value);
    sprintf(buffer, "%f\n", *tmpptr);
  }
  else if(p->flags & FW_ENUM)
    sprintf(buffer, "%s\n", (char *) p->value);
  else if(p->flags &FW_CHAR) {
    if(p->flags &FW_POINTER)
      sprintf(buffer, "\"%s\"\n", (char *) p->value);
    else if(p->flags &FW_SIGNED){
      char *tmpptr = (char *)&(p->value);
      char tmpvalue = *tmpptr;
      sprintf(buffer, "%c\n", tmpvalue);
    }
    else{
      unsigned char *tmpptr = (unsigned char *)&(p->value);
      unsigned char tmpvalue = *tmpptr;
      sprintf(buffer, "%hhu\n", *tmpptr);
    }
  }
  else if(p->flags & FW_STRUCT && p->flags & FW_POINTER)
    sprintf(buffer, "%p\n", p->value);
  else if(p->flags & FW_STRUCT)
    sprintf(buffer, "%p\n", p->addr);
  else
    sprintf(buffer, "0x%lx\n", p->value);
}

