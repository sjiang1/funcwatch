#include "funcwatch.h"
#include "funcwatch_output.h"
#include "vector.h"
#include "dynstring.h"
#include "stringutil.h"
#include <inttypes.h>

void output_logged_values(FILE *f, funcwatch_run *run){
  if(DEBUG){
    fprintf(stderr, "Total Call #%d\n", run->num_calls);
    fprintf(stderr, "Total Rets #%d\n", run->num_rets);
  }
  
  fprintf(f, "%s, %s, %s, %s, %s, %s, %s, %s \n",
	  "Is Return Flag", "Function", "Call Number",
	  "Variable Name", "Variable Size",
	  "Usage Flags", "Variable Type", "Value");
  
  DynString toPrint = output_logged_values_inner(run);
  fprintf(f, "%s", toPrint.text);
  dynstring_inner_free(toPrint);
}

DynString output_logged_values_inner(funcwatch_run *run){
  DynString toPrint;
  dynstring_init(&toPrint);
  
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
    else if(run->params[0].size == 0 || run->ret_params[0].size == 0){
      // later, we need to find param based on the curr_call_id
      // if the params/ret_params are empty, we cannot find the param
      // will cause a segmentation error when we assume
      // that there always should be a param found'
      hasParams = 0;
    }

    
    int hasReturn = 1;
    if(run->return_values == NULL) {
      if(DEBUG)
	fprintf(stderr, "function %s does not have return value.\n", run->func_name);
      hasReturn = 0;
    }
    
    if(hasParams){
      // print input values
      Vector callparams = run->params[curr_call_id];
      DynString str = print_param_vector(callparams, 0);
      dynstring_append(&toPrint, str.text);
      dynstring_inner_free(str);
    }
    
    if(hasReturn){
      // print return
      funcwatch_param *r = get_return_of_call_id(run->return_values, run->num_rets, curr_call_id);
      DynString str = print_param_list(r, 1);
      dynstring_append(&toPrint, str.text);
      dynstring_inner_free(str);
    }
    
    if(hasParams){
      // print parameters values when function returns
      Vector *ret_p = get_param_of_call_id(run->ret_params, run->num_calls, curr_call_id);
      DynString str = print_param_vector(*ret_p, 1);
      dynstring_append(&toPrint, str.text);
      dynstring_inner_free(str);
    }
  }

  return toPrint;
}

DynString print_param_list(funcwatch_param *p, int is_return){
  DynString listString;
  dynstring_init(&listString);
  
  while(p != NULL) {
    if(DEBUG)
      fprintf(stderr, "print param list, head: %s\n", p->name);

    DynString paramString = print_param(p, is_return);
    dynstring_append(&listString, paramString.text);
    // print_param will pad a new line character in the end of paramString.
    dynstring_inner_free(paramString);
    
    p = p->next;
  }
  
  return listString;
}

DynString print_param_vector(Vector params, int is_return_param){
  if(DEBUG)
    fprintf(stderr, "print param vector\n");

  DynString toPrint;
  dynstring_init(&toPrint);
  
  for(int i =0; i<params.size; i++){
    funcwatch_param *p = vector_get(&params, i);
    DynString paramString = print_param_list(p, is_return_param);
    dynstring_append(&toPrint, paramString.text);
    dynstring_inner_free(paramString);
  }
  
  return toPrint;
}

DynString print_param_list_for_return(funcwatch_param *p){
  return print_param_list(p, 1);
}

#define MAKESTR(x) # x
#define STR(x) MAKESTR(x)

/*
 * Print format:
 * is_return, function_name, call_id, parameter_name, parameter_size, flags, parameter_type, value, 
 */
DynString print_param(funcwatch_param *p, int is_return) {
  DynString paramString;
  dynstring_init(&paramString);

  int bufferSize = 1024;
  char buffer [1024];
  // print is return, has sub-values, function name, call num, parameter name, parameter size
  snprintf(buffer, bufferSize, "%d, %s, %d, %s, %zu,", is_return, p->func_name, p->call_num, p->name, p->size);
  dynstring_append(&paramString, buffer);

  char flagbuffer[33];
  flagbuffer[32] = '\0';
  int2bin(p->flags, flagbuffer, 32);  
  snprintf(buffer, bufferSize, " %s,", flagbuffer);
  dynstring_append(&paramString, buffer);

  // print parameter type
  snprintf(buffer, bufferSize, " %s,", p->type);
  dynstring_append(&paramString, buffer);

  // print parameter value
  if(p->type != NULL &&
     (strcmp(p->type, "unsupported")==0 || (p->flags & FW_INVALID))){
    snprintf(buffer, bufferSize, " [invalid]\n");
    dynstring_append(&paramString, buffer);
  }
  else if((p->flags & FW_POINTER) && (p->value == 0)){
    // null pointers
    snprintf(buffer, bufferSize, " %s\n", "null");
    dynstring_append(&paramString, buffer);
  }
  else if(p->flags & FW_POINTER && p->flags & FW_CHAR){
    char * val = 0;
    memcpy(&val, &(p->value), sizeof(char *));
    snprintf(buffer, bufferSize, " \"%s\"\n", val);
    dynstring_append(&paramString, buffer);
  }
  else if(p->flags & FW_POINTER){
    // non-null pointers
    if(!PRINT_MEM_ADDR){
      snprintf(buffer, bufferSize, " %s\n", "[memory addr]");
    }
    else{
      snprintf(buffer, bufferSize, " %p\n", (void *)p->value);
    }
    dynstring_append(&paramString, buffer);
  }
  else if((p->flags & FW_INT) && (p->flags & FW_SIGNED)) {
    // signed int
    if(p->size == 2){
      short val = 0;
      memcpy(&val, &(p->value), sizeof(short));
      snprintf(buffer, bufferSize, " %d\n", val);
    }else if(p->size == 4){
      int32_t val = 0;
      memcpy(&val, &(p->value), sizeof(int32_t));
      // snprintf(buffer, bufferSize, " %0" STR(11) PRId32  "\n", val);
      snprintf(buffer, bufferSize, " %" PRId32  "\n", val);
    }else{
      // default behavior
      int64_t val = 0;
      memcpy(&val, &(p->value), sizeof(int64_t));
      // snprintf(buffer, bufferSize, " %0" STR(20) PRId64 "\n", val);
      snprintf(buffer, bufferSize, " %" PRId64 "\n", val);
    }
    dynstring_append(&paramString, buffer);
  }
  else if(p->flags & FW_INT){
    // unsigned int
    if(p->size == 2){
      unsigned short val = 0;
      memcpy(&val, &(p->value), sizeof(unsigned short));
      snprintf(buffer, bufferSize, " %hu\n", val);
    }else if(p->size == 4){
      uint32_t val = 0;
      memcpy(&val, &(p->value), sizeof(uint32_t));
      snprintf(buffer, bufferSize, " %" PRIu32 "\n", val);
    }else {
      // default behavior
      uint64_t val = 0;
      memcpy(&val, &(p->value), sizeof(uint64_t));
      snprintf(buffer, bufferSize, " %" PRIu64 "\n", val);
    }
    dynstring_append(&paramString, buffer);
  }
  else if((p->flags &FW_CHAR) && (p->flags &FW_SIGNED)) {
    // signed char
    char value = 0;
    memcpy(&value, &p->value, sizeof(char));
    snprintf(buffer, bufferSize, " %d\n", value);
    dynstring_append(&paramString, buffer);
  }
  else if(p->flags &FW_CHAR){
    // unsigned char
    unsigned char value = 0;
    memcpy(&value, &p->value, sizeof(unsigned char));
    snprintf(buffer, bufferSize, " %d\n", value);
    dynstring_append(&paramString, buffer);
  }
  else if(p->flags & FW_FLOAT) {
    // float
    if( p->size == sizeof(float) ){
      float value = 0;
      memcpy(&value, &p->value, sizeof(float));
      snprintf(buffer, bufferSize, " %f\n", value);
      dynstring_append(&paramString, buffer);
    }else if( p->size == sizeof(double) ){
      double value = 0;
      memcpy(&value, &p->value_float, sizeof(double));
      snprintf(buffer, bufferSize, " %f\n", value);
      dynstring_append(&paramString, buffer);
    }else{
      // default behavior
      snprintf(buffer, bufferSize, " %f\n", p->value_float);
      dynstring_append(&paramString, buffer);
    }
  }
  else if(p->flags & FW_ENUM){
    // enum
    snprintf(buffer, bufferSize, " %s\n", (char *) p->value);
    dynstring_append(&paramString, buffer);
  }
  else if(p->flags & FW_STRUCT || p->flags & FW_UNION){
    // struct value is illustrated by its fields
    snprintf(buffer, bufferSize, " \n");
    dynstring_append(&paramString, buffer);
  }
  else{
    // default
    snprintf(buffer, bufferSize, " no flag to identify the type\n");
    dynstring_append(&paramString, buffer);
  }

  return paramString;
}

