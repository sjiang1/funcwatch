#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "commandhandle.h"
#include "funcwatch.h"
#include "funcwatch_output.h"
#include "util.h"

const char *result_folder = "./FuncwatchResults";
static void print_outfile_name(char *outfile, int argc, char *argv[]);
static int check_run(funcwatch_run *run);

int main(int argc, char *argv[]) {
  if(argc < 3) { 
    fprintf(stderr, "Usage 1: logging only one function's I/O values. Input the name of the program that to be logged.\n");
    fprintf(stderr, "         funcwatch <function name> <program name> [program arguments]\n");
    fprintf(stderr, "         e.g. \n");
    fprintf(stderr, "         ./funcwatch foo tests/test_pointer_0_int\n");
    fprintf(stderr, "Usage 2: logging multiple functions' I/O values. Input a file that contains the functions' names.\n");
    fprintf(stderr, "         funcwatch :<functionnamelistfile> <program name> [program arguments]\n");
    fprintf(stderr, "         e.g. \n");
    fprintf(stderr, "         ./funcwatch :example_funcnames.txt tests/test_pointer_0_int\n");
    return 0;
  }

  Vector target_functions;
  vector_init(&target_functions);
  if(argv[1][0] == ':'){
    printf("Multi-functions logging from %s \n", argv[1]);
    // malloced inside read_functionnames
    read_functionnames(&target_functions, (argv[1]+1)); 
    return; //not ready for using the multi-functions logging.
  }else{
    printf("Logging one function: %s\n", argv[1]);
    size_t funcname_len = strlen(argv[1]);
    char *data_to_save = (char *)malloc(sizeof(char) *(funcname_len + 1));
    strncpy(data_to_save, argv[1], funcname_len + 1);
    vector_append(&target_functions, data_to_save);
  } // target_functions should be freed in the end of the main function

  // there is something that makes memory not safe to call fopen after we get funcwatch_run
  funcwatch_run *run = funcwatch_run_program(argv[2], argv[1], &argv[2]);
  int run_status = check_run(run);
  if(run_status < 0)
    return 1;
  
  char outfile[500]; 
  print_outfile_name(outfile, argc, argv);
  FILE *f = fopen(outfile, "w");
  if(f == 0){
    printf("Cannot open: %s", outfile);
    return 1;
  }

  output_logged_values(f, run);
  fclose(f);
  vector_inner_free(&target_functions);
  //dwarf_finish, dwarf_init should be here.
  return 0;
}

/*
 * return -1, or 0 if run does not have any value logged.
 */
static int check_run(funcwatch_run *run){
  if(run == 0)
    return -1;
  if(run->num_calls == 0) {
    fprintf(stderr, "Warning: function %s was never called. No output will be created.\n", run->func_name);
    return 0;
  }
  return 1;
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
