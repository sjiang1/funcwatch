#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "funcwatch.h"

const char *result_folder = "./FuncwatchResults";
static void print_param(FILE *f, funcwatch_param *p, int is_return);
static void print_outfile_name(char *outfile, int argc, char *argv[]);

int main(int argc, char *argv[]) {
  if(argc < 3) { 
    printf("%s", "Usage: funcwatch <function name> <program name> [program arguments]\n");
    return 0;
  }

  // there is something that makes memory not safe to call fopen after we get funcwatch_run
  funcwatch_run *run = funcwatch_run_program(argv[2], argv[1], &argv[2]);
  if(run == 0)
    return 0;
  if(run->num_calls == 0) {
    fprintf(stderr, "Warning: function %s was never called. No output will be created.", run->func_name);
    return 0;
  }

  char outfile[500]; 
  print_outfile_name(outfile, argc, argv);
  FILE *f = fopen(outfile, "w");
  if(f == 0){
    printf("Cannot open: %s", outfile);
    return 0;
  }
   
  fprintf(f, "%s\n", "Is Return Flag, Function, Call Number, Variable Name, Variable Size, Usage Flags, Variable Type, Value ");
  for(int i = 0; i < run->num_calls; ++i)  {
    // print input valuse
    funcwatch_param *p = run->params[i];
    while(p != NULL) {
      print_param(f, p, 0);
      p = p->next;
    }
    // print return
    if(run->return_values != NULL) {
      funcwatch_param r = run->return_values[i];
      funcwatch_param *p = &r;
      print_param(f, p, 1);
      while(p->next != NULL) {
	p = p->next;
	print_param(f, p, 1);
      }
    }
    // print parameters values when function returns
    funcwatch_param *ret_p = run->ret_params[i];
    while(ret_p != NULL) {
      print_param(f, ret_p, 1);
      ret_p = ret_p->next;
    }
  }

  fclose(f);
  //dwarf_finish, dwarf_init should be here.
  return 0;
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
  }
  else if(p->flags & FW_FLOAT) {
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

static void print_outfile_name(char *outfile, int argc, char *argv[]){
  const char *illegalChars = ".\\/:?\"<>|";
  const int illegalChars_count = strlen(illegalChars);

  char program_name_buffer[150];
  program_name_buffer[0] = 0;
  strcpy(program_name_buffer, argv[2]);
  for(int i=0; i < strlen(program_name_buffer); i++){
    for(int j=0; j < illegalChars_count; j++){
      if(program_name_buffer[i] == illegalChars[j]){
	program_name_buffer[i] = '_';
	break;
      }	
    }
  }  
  
  char arguments_buffer[150];
  arguments_buffer[0] = 0;
  int remaining_arguments_count = argc - 3;
  int visited_arguments_count = 0;
  int first_argument = 1;
  while(remaining_arguments_count > 0){
    // replace the illegal character with '.'
    char current_argument[30];
    strcpy(current_argument, argv[3+visited_arguments_count]);
    int argument_length = strlen(current_argument);
    for(int i = 0; i < argument_length; i++){
      for(int j = 0; j < illegalChars_count; j++){
	if(current_argument[i] == illegalChars[j]){
	  current_argument[i] = '.';
	  break;
	}
      }
    }

    if(first_argument){
      strcpy(arguments_buffer, "-");
      strcat(arguments_buffer, current_argument);
      first_argument = 0;
    }
    else{
      strcat(arguments_buffer, "-");
      strcat(arguments_buffer, current_argument);
    }
    visited_arguments_count ++;
    remaining_arguments_count --;
  }

  struct stat st = {0};
  if(stat(result_folder, &st) == -1){
    mkdir(result_folder, 0700);
  }

  char result_folder_function_folder[80];
  result_folder_function_folder[0] = 0;
  strcpy(result_folder_function_folder, result_folder);
  strcat(result_folder_function_folder, "/");
  strcat(result_folder_function_folder, argv[1]);
  
  if(stat(result_folder_function_folder, &st) == -1){
    mkdir(result_folder_function_folder, 0700);
  }
  strcat(result_folder_function_folder, "/");
  
  sprintf(outfile, "%s%s%s.csv", result_folder_function_folder, program_name_buffer, arguments_buffer);
  printf("outfile: %s\n", outfile);

  return;
}
