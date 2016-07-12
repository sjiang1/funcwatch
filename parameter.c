#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parameter.h"
#include "util.h"


// functions - value
void value_list_init(struct value_list *p){
  vector_init(&(p->run_names));
  vector_init(&(p->call_ids));
  vector_init(&(p->values));
}

void value_list_inner_free(struct value_list *p){
  vector_inner_free(&(p->run_names));
  vector_inner_free(&(p->call_ids));
  vector_inner_free(&(p->values));
}

int find_matched_value_in_value_list(const char *run_name, int call_id, struct value_list values){
  int matched_index = -1;
  for(int i=0; i<values.call_ids.size; i++){
    int *tmp_call_id = vector_get(&values.call_ids,i);
    char *tmp_run_name = vector_get(&values.run_names, i);
    if(*tmp_call_id == call_id
       && strcmp(tmp_run_name, run_name)==0){
      matched_index = i;
      break;
    }
  }
  return matched_index;
}

int value_list_need_one_value(struct value_list values){
  int call_id_count = values.call_ids.size;
  int value_count = values.values.size;
  return call_id_count == value_count + 1;
}

void printf_value_list(struct value_list values){
  for(int i=0; i<values.values.size;i++){
    char *value = vector_get(&values.values, i);
    int *call_id = vector_get(&values.call_ids,i);
    char *run_name = vector_get(&values.run_names,i);
    printf("\t%s\t%d\t%s\n", run_name, *call_id, value);
  }
}

int value_list_has_null(struct value_list values){
  for(int i=0; i<values.values.size;i++){
    char *value = vector_get(&values.values, i);
    if(strcmp(value, "NULL") == 0
       || strcmp(value, "null") == 0
       || strcmp(value, "0") == 0)
      return 1;
  }
  return 0;
}

void changed_value_list_init(struct changed_value_list *p){
  vector_init(&(p->run_names));
  vector_init(&(p->call_ids));
  vector_init(&(p->before_values));
  vector_init(&(p->after_values));
}

void changed_value_list_inner_free(struct changed_value_list *p){
  vector_inner_free(&(p->run_names));
  vector_inner_free(&(p->call_ids));
  vector_inner_free(&(p->before_values));
  vector_inner_free(&(p->after_values));
}

int changed_value_list_need_one_value_in_after(struct changed_value_list values){
  int call_id_count = values.call_ids.size;
  int value_count = values.after_values.size;
  return call_id_count == value_count + 1;
}

void printf_changed_value_list(struct changed_value_list values){
  for(int i=0; i<values.after_values.size;i++){
    char *before_value = vector_get(&values.before_values, i);
    char *after_value = vector_get(&values.after_values, i);
    int *call_id = vector_get(&values.call_ids,i);
    char *run_name = vector_get(&values.run_names,i);
    printf("\t%s\t%d\t%s\t%s\n", run_name, *call_id, before_value, after_value);
  }
}

int changed_value_list_has_input_null(struct changed_value_list values){
  for(int i=0; i<values.before_values.size;i++){
    char *value = vector_get(&values.before_values, i);
    if(strcmp(value, "NULL") == 0
       || strcmp(value, "null") == 0
       || strcmp(value, "0") == 0)
      return 1;
  }
  return 0;
}

// functions - parameter
void parameter_init(struct parameter_entry *p){
  p->name = 0;
  p->type = 0;

  value_list_init(& (p->input_values));
  value_list_init(& (p->output_values));
  changed_value_list_init(& (p->changed_values));
  
  vector_init(&(p->members));
}

void parameter_inner_free(struct parameter_entry *p){
  if(p->name)
    free(p->name);
  if(p->type)
    free(p->type);

  value_list_inner_free(& (p->input_values));
  value_list_inner_free(& (p->output_values));
  changed_value_list_inner_free(& (p->changed_values));

  for(int i=0; i<p->members.size; i++){
    struct parameter_entry *member = vector_get(&(p->members), i);
    parameter_inner_free(member);
  }
  vector_inner_free(& (p->members));
}

int parameter_name_is_under(const char *name, const char *tmp_name){
  size_t name_len = strlen(name);
  size_t tmp_len = strlen(tmp_name) + 1; // trailing zero
  if(name_len <= tmp_len){
    return 0;
  }
  
  int ret = 0;
  char *tmp_name_dot = malloc(sizeof(char) * (tmp_len + 1));
  strcpy(tmp_name_dot, tmp_name);
  tmp_name_dot[tmp_len - 1]='.';
  tmp_name_dot[tmp_len] = '\0';
  if(starts_with(name, tmp_name_dot)){
    ret = 1;
  }
  free(tmp_name_dot);
  return ret;
}

struct parameter_entry *find_matched_parameter_in_array(const char *name, Vector *parameter_arr){
  struct parameter_entry *matched_parameter = NULL;
  
  for(int i=0; i<parameter_arr->size; i++){
    struct parameter_entry *tmp_parameter = vector_get(parameter_arr, i);
    char *tmp_name = tmp_parameter->name;
    if(strcmp(name, tmp_name) == 0){
      matched_parameter = tmp_parameter;
      break;
    }
    if(parameter_name_is_under(name, tmp_name)){
      matched_parameter = find_matched_parameter_in_array(name, &(tmp_parameter->members));
      if(matched_parameter == NULL){
	// this name should be a member parameter in tmp_name
	matched_parameter = malloc(sizeof(struct parameter_entry));
	parameter_init(matched_parameter);
	matched_parameter->name = strcpy_deep(name);
	vector_append(&(tmp_parameter->members), matched_parameter);
      }
      break;
    }
  }
  return matched_parameter;
}

int parameter_can_be_null(struct parameter_entry p){
  if(value_list_has_null(p.input_values))
    return 1;
  if(changed_value_list_has_input_null(p.changed_values))
    return 1;
  return 0;
}

void parameter_printf_random_value(struct parameter_entry* p, FILE* output_file){
  int inputvalues_cnt = p->input_values.values.size;
  int outputvalues_cnt = p->output_values.values.size;
  int changedvalues_cnt = p->changed_values.before_values.size;
  int allcnt = inputvalues_cnt + outputvalues_cnt + changedvalues_cnt;
  // srand is called in summarize_results.c;
  int r = rand();
  int random_number = r % allcnt + 0;
  if(random_number < inputvalues_cnt){
    char * value = vector_get(&(p->input_values.values), random_number);
    fprintf(output_file, "%s\t%s\n", p->name, value);
  }else if(random_number < inputvalues_cnt + outputvalues_cnt){
    char * value = vector_get(&(p->output_values.values), random_number - inputvalues_cnt);
    fprintf(output_file, "%s\t%s\n", p->name, value);
  }else if(random_number < allcnt){
    char * value = vector_get(&(p->changed_values.before_values), random_number - inputvalues_cnt - outputvalues_cnt);
    fprintf(output_file, "%s\t%s\n", p->name, value);
  }  
  return;
}
