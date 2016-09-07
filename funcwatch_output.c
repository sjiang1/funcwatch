#include "funcwatch.h"
#include "funcwatch_output.h"
#include "util.h"

static void print_param(FILE *f, funcwatch_param *p, int is_return);
static funcwatch_param *get_return_from_call_id(funcwatch_run *run, int curr_call_id);

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
    
    // print input values
    if(run->params == 0){
      fprintf(stderr, "Warning: function %s does not have parameters.\n", run->func_name);
    }else{
      funcwatch_param *p = run->params[i];
      while(p != NULL) {
	if(DEBUG)
	  fprintf(stderr, "print param: %s\n", p->name);
	
	print_param(f, p, 0);
	p = p->next;
      }
    }
    
    // print return
    if(run->return_values == NULL) {
      fprintf(stderr, "Warning: function %s does not have return value.\n", run->func_name);
    }
    else{
      funcwatch_param *p = get_return_from_call_id(curr_call_id);
      if(DEBUG)
	fprintf(stderr, "print return: %s\n", p->name);
      print_param(f, p, 1);
      
      while(p->next != NULL) {
	p = p->next;
	if(DEBUG)
	  fprintf(stderr, "print param: %s\n", p->name);

	print_param(f, p, 1);
      }
    }
    
    // print parameters values when function returns
    if(run->ret_params != 0){
      funcwatch_param *ret_p = run->ret_params[i];
      while(ret_p != NULL) {
	if(DEBUG)
	  fprintf(stderr, "print param after run: %s\n", ret_p->name);

	print_param(f, ret_p, 1);
	ret_p = ret_p->next;
      }
    }
  }

  return;
}

static funcwatch_param *get_return_from_call_id(funcwatch_run *run, int call_id){
  return &(run->return_values[i]);
}

static void print_param(FILE *f, funcwatch_param *p, int is_return) {
  // print is return, has sub-values, function name, call num, parameter name, parameter size, flags
  fprintf(f, "%d, %s, %d, %s, %zu,", is_return, p->func_name, p->call_num, p->name, p->size);
  fprintf(f, " 0x%x,", p->flags);

  if(p->type == 0) p->type = " ";
  // print parameter type
  if(p->flags & FW_POINTER)
    fprintf(f, "%s *,", p->type);
  else
    fprintf(f, "%s,", p->type);

  // print parameter value
  if(strcmp(p->type, "unsupported")==0)
    fprintf(f, "unsupported value\n");
  else if(p->flags & FW_INVALID)
    fprintf(f, "0\n");
  else if(p->flags & FW_POINTER
	  && p->value == 0)
    fprintf(f, "%s\n", "NULL");
  else if(p->flags & FW_INT) {
    if(p->flags & FW_SIGNED){
      if(p->flags & FW_POINTER)
	fprintf(f, "%ld\n", *(long *)p->value);
      else
	fprintf(f, "%ld\n", p->value);
    }
    else{
      if(p->flags & FW_POINTER){
	unsigned long *tmpptr = p->value;
	unsigned long tmpvalue = *tmpptr;
	fprintf(f, "%lu\n", tmpvalue);
      }else{
	unsigned long *tmpptr = &(p->value);
	unsigned long tmpvalue = *tmpptr;
	fprintf(f, "%lu\n", tmpvalue);
      }
    }
  }else if(p->flags & FW_FLOAT) {
    float *tmpptr = 0;
    if(p->flags & FW_POINTER)
      tmpptr = p->value;
    else
      tmpptr = &(p->value);
    fprintf(f, "%f\n", *tmpptr);
  }
  else if(p->flags & FW_ENUM)
    fprintf(f, "%s\n", (char *) p->value);
  else if(p->flags &FW_CHAR) {
    if(p->flags &FW_POINTER)
      fprintf(f, "\"%s\"\n", (char *) p->value);
    else if(p->flags &FW_SIGNED){
      char *tmpptr = (char *)&(p->value);
      char tmpvalue = *tmpptr;
      fprintf(f, "%c\n", tmpvalue);
    }
    else{
      unsigned char *tmpptr = (unsigned char *)&(p->value);
      unsigned char tmpvalue = *tmpptr;
      fprintf(f, "%hhu\n", *tmpptr);
    }
  }
  else if(p->flags & FW_STRUCT && p->flags & FW_POINTER)
    fprintf(f, "%p\n", p->value);
  else if(p->flags & FW_STRUCT)
    fprintf(f, "%p\n", p->addr);
  else
    fprintf(f, "0x%lx\n", p->value);
}

