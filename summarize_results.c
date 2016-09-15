#include <stdio.h>
#include <stdlib.h>
#include <dirent.h> 
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <limits.h>
#include <debug_printf.h>
#include <time.h>

// third party library
#include "csv.h"

// headers for our own code
#include "vector.h"
#include "stringutil.h"
#include "parameter.h"

// process the results for one function
void process_results_for_one_function(const char *funcwatch_result_directory);
int clean_one_result_file(const char *file_name);
void process_one_result_file(const char *file_name, Vector *parameter_arr);

// Utility functions
int is_csv_file(const char *path);
int is_subfolder(const char *path);
int is_headline(const char *line);
DIR *open_input_directory(const char*dir);
int get_sub_name_and_real_path(struct dirent *dir_ent, const char* parent_dir, char *output_sub_name, char *output_sub_real_path);
int get_function_name_and_real_path(struct dirent *dir_ent, const char* parent_dir, char *output_function_name, char *output_real_path);
int try_open_file_and_get_first_line(const char *file_name, const char *file_real_path, char **line_p, int *read_char_number,FILE **fpp);
void global_variables_init(const char *file_name);
void copy_head_from_file(const char *source_file, const char*dest_file, int end_line_number);
void make_sure_new_line_ends(char *line);
// Utility functions - output results
void printf_parameter_ids(Vector parameter_arr, FILE* output_file);
void printf_io_examples(Vector parameter_arr, FILE* output_file);

// Callback functions for libcsv - declaration
// Callback functions for libcsv - declaration - for clean_one_result_file
void count_field_number(void *field, size_t field_len, void *output);
void reset_field_number(int last_char, void *output);
// Callback functions for libcsv - declaration - for process_one_result_file
void end_of_field_processor(void *field, size_t field_len, void *output);
void end_of_row_processor(int last_char, void *output);

// const strings
const char *help =
  "Please enter a directory holds only the funcwatch results.\nE.g. ./funcwatch_results/. This folder is automatically generated by funcwatch.\n";
const char *return_name = "$return_value"; //this is defined by funcwatch

// Global variables
// it stores the current function name
// it is only updated in main function, before process_results_for_one_function
char *current_function_name;
// it stores the current function id
// it is only updated in main function, before process_results_for_one_function
int current_function_id;

/* the following global variables should be initialized before csv_parser is called
 * They should be changed only in:
 *    end_of_field_processor and end_of_row_processor */
// it counts the index of the current field, if it is -1, marks this current record is invalid
int global_field_count;
// it marks whether the value should be a return or not
int return_flag;
// it stores the current processing parameter
struct parameter_entry *current_parameter;
// it marks whether the current_parameter exists in parameter_arr
int in_parameter_arr;
// it marks whether the current_parameter starts a new call record
int previous_call_id;
// it stores the current run name
char *current_run_name;

int main(int argc, char *argv[]){
  if(argc < 2){
    printf("%s", help);
    return 0;
  }
  const char *funcwatch_result_directory = argv[1];
  DIR* dir = opendir(funcwatch_result_directory);
  if (dir == NULL){
    fprintf(stderr, "opendir");
    exit(EXIT_FAILURE);
  }

  // seed random generator
  srand(time(NULL));
  
  // loop: read all the sub-folders in funcwatch result folder
  // each folder stores the funcwatch result for one function
  struct dirent* dir_ent;
  current_function_id = 0;
  while((dir_ent = readdir(dir)) != NULL){
    char function_name[PATH_MAX + 1];
    char real_path[PATH_MAX + 1];
    int ret = get_function_name_and_real_path(dir_ent, funcwatch_result_directory, function_name, real_path);
    if(ret == 0) continue; // go to next file in the directory
    // check if the file is csv file (the files generated by funcwatch should be csv files)
    int is_valid_funcwatch_sub_folder = is_subfolder(real_path);
    if(!is_valid_funcwatch_sub_folder){
      printf ("Skip funcwatch result subfolder: [%s]\n", real_path);
      continue; // read next sub-folder
    }
    printf("Processing for function: %s\n", function_name);
    current_function_name = function_name;
    process_results_for_one_function(real_path); 
    current_function_id += 1;
  } // loop end: read next sub-folder in funcwatch result folder
  closedir(dir);

  exit(EXIT_SUCCESS);
}

void process_results_for_one_function(const char *one_function_result_directory){
  int ret = 0;
  DIR *d = open_input_directory(one_function_result_directory);
  
  // Loop: read every file from the directory - nonrecursive
  // we assume all the files in this directory should be .csv generated by funcwatch
  struct dirent *dir_ent;

  // a dynamic array of parameter_entry
  Vector parameter_arr; // all the values (across various files) stores in parameter_arr
  vector_init(&parameter_arr);
  while ((dir_ent = readdir(d)) != NULL){
    char file_name[PATH_MAX + 1];
    char file_real_path[PATH_MAX + 1];
    ret = get_sub_name_and_real_path(dir_ent, one_function_result_directory, file_name, file_real_path);
    if(ret == 0) continue; // go to next file in the directory
    // check if the file is csv file (the files generated by funcwatch should be csv files)
    int is_valid_input = is_csv_file(file_real_path);
    if(!is_valid_input){
      printf ("Skip non-csv-file: [%s]\n", file_real_path);
      continue; // go to next file in the directory
    }

    // clean the file first
    ret = clean_one_result_file(file_real_path);
    if(ret == -1) continue; // go to the next file in the directory
    // start processing this file
    process_one_result_file(file_real_path, &parameter_arr);
  } // end loop: read every file from the directory

  closedir(d);

  FILE *output_fp = NULL;
  output_fp = fopen ("parameterids.txt", "a" );
  printf_parameter_ids(parameter_arr, output_fp);
  fclose(output_fp);
  char output_name[80];
  strcpy(output_name, "./ioexamples/");
  struct stat st={0};
  if(stat("./ioexamples", &st) == -1){
    mkdir("./ioexamples", 0700); 
  }
  strcat(output_name, current_function_name);
  strcat(output_name, ".ioexample");
  output_fp = fopen (output_name, "w");
  printf_io_examples(parameter_arr, output_fp);
  fclose(output_fp);
  output_fp = NULL;
  // free vector parameter_arr
  for(int i=0; i<parameter_arr.size; i++){
    struct parameter_entry* p = vector_get(&parameter_arr, i);
    parameter_inner_free(p);
  }
  vector_inner_free(&parameter_arr);
}

int clean_one_result_file(const char *file_name){
  printf ("Cleaning: [%s]\n", file_name);
  char * line = NULL;
  FILE * fp = NULL;
  ssize_t read_char_number = 0;
  int ret = 0;
  ret = try_open_file_and_get_first_line(file_name, file_name, &line, &read_char_number, &fp);
  if(ret == -1) return -1;
  
  size_t len = 0;
  // prepare to use libcsv to parse the lines
  struct csv_parser csv_p;
  csv_init(&csv_p, 0);
  int record_data[5];
  record_data[0] = 0; // field count
  record_data[1] = 0; // line number
  record_data[2] = -1; // call id
  record_data[3] = -1; // previous  call id
  record_data[4] = 1; // first line of the call id
  int need_to_clean = 0;
  do{ // process the file line by line
    if(len < 3) continue;
    char *new_line = malloc(sizeof(char) * (len + 2));
    strcpy(new_line, line);
    make_sure_new_line_ends(new_line);
    // printf("Retrieved line of length %zu :\n", read_char_number);
    // printf("%s", new_line);
    int parse_char_number = csv_parse(&csv_p, new_line, read_char_number, count_field_number, reset_field_number, record_data);
    free(new_line);
    
    if(parse_char_number == read_char_number){
      if(record_data[0] == -1){
	printf("%s", new_line);
	need_to_clean = 1;
	break; // we do not need to read the rest (there is an incomplete line)
      }
      record_data[1] = record_data[1] + 1;
      continue; // go to the next line
    }
    // if parse_char_number != read_char_number
    // parser fails
    fprintf(stderr, "Error parsing file: [%s]\n line: %s\n", file_name, line);
    exit(EXIT_FAILURE);
  } // end loop: read the file line by line
  while ((read_char_number = getline(&line, &len, fp)) != -1);

  if(record_data[0] != 8 && record_data[0] != 0){
    printf("line: %s", line);
    printf("field count : %d\n", record_data[0]);
    need_to_clean = 1;
  }

  if (line) // free the buffer that getline creates
    free(line);
  fclose(fp); // close the file
  csv_free(&csv_p); // free libcsv resources

  if(!need_to_clean) return;

  errno = 0;
  // back up the original file  
  char backup_filename[PATH_MAX+1];
  strcpy(backup_filename, file_name);
  strcat(backup_filename, ".bak");
  ret = rename(file_name, backup_filename);
  if(ret == -1){
    debug_printf("ERROR: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }
  // copy the complete call-id records to the new file
  copy_head_from_file(backup_filename, file_name, record_data[4]);
  return;
}

// Callback functions for libcsv
// Callback functions for libcsv - for clean_one_result_file
void count_field_number(void *field, size_t field_len, void *output){
  int *record = (int *)output;
  if(record[0] == 2){
    // get the string of the field
    char *field_str = malloc((field_len + 1) * sizeof(char));
    memcpy(field_str, field, field_len);
    field_str[field_len] = '\0';

    long long_id = strtol(field_str, NULL, 10);
    record[3] = record[2]; //update previous call id
    record[2] = long_id; // call id
    if(record[2] != record[3])
      record[4] = record[1]; // first line of the call id
    free(field_str);
  }
  record[0] = record[0] + 1; // field number
}

void reset_field_number(int last_char, void *output){
  int *record = (int *)output;
  if(record[0] != 8)
    record[0] = -1;
  else
    record[0] = 0;
}

// Process one funcwatch result file
void process_one_result_file(const char *file_name, Vector *parameter_arr){
  printf ("Processing: [%s]\n", file_name);
  char * line = NULL;
  FILE * fp = NULL;
  ssize_t read_char_number = 0;
  int ret = try_open_file_and_get_first_line(file_name, file_name, &line, &read_char_number, &fp);
  if(ret == -1) return; // go to next file in the directory
  
  size_t len = strlen(line);
  // prepare to use libcsv to parse the lines
  struct csv_parser csv_p;
  csv_init(&csv_p, 0);
  global_variables_init(file_name);
  int line_number = 0;
  do{ // process the file line by line
    if(len < 3) continue;
    char *new_line = malloc(sizeof(char) * (len + 2));
    strcpy(new_line, line);
    make_sure_new_line_ends(new_line);
    // printf("Retrieved line of length %zu :\n", read_char_number);
    // printf("\n%d: %s", line_number, new_line);
    int parse_char_number = csv_parse(&csv_p, new_line, read_char_number, end_of_field_processor, end_of_row_processor, parameter_arr);
    free(new_line);
    line_number ++;
    if(parse_char_number == read_char_number)
      continue; // go to the next line
    // if parse_char_number != read_char_number
    // parser fails
    fprintf(stderr, "Error parsing file: [%s]\n line: %s\n", file_name, line);
    exit(EXIT_FAILURE);
  } // end loop: read the file line by line
  while ((read_char_number = getline(&line, &len, fp)) != -1);
  
  if (line) // free the buffer that getline creates
    free(line);
  fclose(fp); // close the file
  
  csv_free(&csv_p); // free libcsv resources
  return;
}

// Callback functions for libcsv
void end_of_field_processor(void *field, size_t field_len, void *output){
  Vector *parameter_arr = output;

  if(global_field_count != -1 && current_parameter == 0){
    current_parameter = malloc(sizeof(struct parameter_entry));
    parameter_init(current_parameter);
  }
  
  // get the string of the field
  char *field_str = malloc((field_len + 1) * sizeof(char));
  memcpy(field_str, field, field_len);
  field_str[field_len] = '\0';
  // printf("%s\t", field_str);
  
  // use the string to set the corresponding field
  switch(global_field_count){
  case 0:{
    long long_id = strtol(field_str, NULL, 10);
    return_flag = long_id;
    // printf("return: %d\n", return_flag);
    break;}
  case 1:{ // function name
    if(strcmp(current_function_name, field_str) != 0){
      global_field_count = -1;
      parameter_inner_free(current_parameter);
      free(current_parameter);
      current_parameter = NULL;
    }
    free(field_str);
    break;}
  case 2:{ // field call id
    long long_id = strtol(field_str, NULL, 10);
    int *call_id = malloc(sizeof(int));
    *call_id = long_id;
    if(*call_id != previous_call_id){
      return_flag = 0;
      previous_call_id = *call_id;
    }

    vector_append(&(current_parameter->input_values.run_names), strcpy_deep(current_run_name));
    vector_append(&(current_parameter->input_values.call_ids), call_id);
    free(field_str);
    break;}
  case 3:{ // field: parameter name
    struct parameter_entry *matched_parameter = find_matched_parameter_in_array(field_str, parameter_arr);
    if(matched_parameter != NULL){
      free(field_str);
      // get the call id/run name from the current parameter
      int *call_id_p = vector_remove_last(&(current_parameter->input_values.call_ids));
      char *run_name = vector_remove_last(&(current_parameter->input_values.run_names));
      // free the current parameter
      parameter_inner_free(current_parameter);
      free(current_parameter);
      // make the matched parameter the current parameter
      current_parameter = matched_parameter;
      in_parameter_arr = 1;
      int matched_value_index = find_matched_value_in_value_list(run_name, *call_id_p, current_parameter->input_values);
      // if there is already a call id in input_values.call_ids, with the same run_name
      // and if the return flag is false
      if(matched_value_index >= 0 && return_flag !=1){
	debug_printf("duplicate call id for one parameter: %s\n", current_parameter->name);
	global_field_count = -1;
	current_parameter = NULL;
      }	
      // if there is already a call id in input_values.call_ids, with the same run_name
      // and if the return flag is true
      // remove the value in input_values and insert it in changed values
      else if(matched_value_index >= 0 && return_flag ==1){
	// get and remove the matched value from input_values
	int *tmp_p1 = vector_remove_at(&(current_parameter->input_values.call_ids),matched_value_index);
	char *tmp_p2 = vector_remove_at(&(current_parameter->input_values.run_names),matched_value_index);
	free(tmp_p1);
	free(tmp_p2);
	
	char *before_value = vector_remove_at(&(current_parameter->input_values.values),matched_value_index);
	// append a new changed value
	vector_append(&(current_parameter->changed_values.run_names), run_name);
	vector_append(&(current_parameter->changed_values.call_ids), call_id_p);
	vector_append(&(current_parameter->changed_values.before_values), before_value);
      }
      // if there is no matched call id in input_values.call_ids, with the same run name
      else if(matched_value_index < 0 && return_flag == 0){
	vector_append(&(current_parameter->input_values.run_names), run_name);
	vector_append(&(current_parameter->input_values.call_ids), call_id_p);
      }
      else if(matched_value_index < 0 && return_flag == 1){
	vector_append(&(current_parameter->output_values.run_names), run_name);
	vector_append(&(current_parameter->output_values.call_ids), call_id_p);
      }
      else{
	fprintf(stderr, "error.\n");
	debug_printf("%s\n", "error"); 
	exit(EXIT_FAILURE);
      }
    }
    else if(current_parameter->name == 0){
      printf("a new parameter: %s\n", field_str);
      current_parameter->name = field_str;
      if(return_flag == 1){
	// get the call id from the current parameter
	int *call_id_p = vector_remove_last(&(current_parameter->input_values.call_ids));
	vector_append(&(current_parameter->output_values.call_ids), call_id_p);
	char *run_name = vector_remove_last(&(current_parameter->input_values.run_names));
	vector_append(&(current_parameter->output_values.run_names), run_name);
      }
    }
    else{
      fprintf(stderr, "error.\n");
      debug_printf("%s\n", "error"); 
      exit(EXIT_FAILURE);}
    break;}
  case 6: // field: parameter type
    if(current_parameter->type == 0)
      current_parameter->type = field_str;
    else
      free(field_str);
    break;
  case 7: {// field: parameter value
    if(return_flag == 0){
      int ret1 = value_list_need_one_value(current_parameter->input_values);
      if(ret1 == 1)
	vector_append(&(current_parameter->input_values.values), field_str);
      else{
	debug_printf("Error: %s\n", "the number of call ids is not matching with the number of input values.");
	exit(EXIT_FAILURE);
      }
    }else{
      // in output_values or in changed_values
      int ret1 = value_list_need_one_value(current_parameter->output_values);
      int ret2 = changed_value_list_need_one_value_in_after(current_parameter->changed_values);
      if(ret1 == 1 && ret2 == 0)
	vector_append(&(current_parameter->output_values.values), field_str);
      else if(ret1 ==0 && ret2 == 1)
	vector_append(&(current_parameter->changed_values.after_values), field_str);
      else{
	debug_printf("%s\n", "error"); 
	exit(EXIT_FAILURE);
      }
    }
    break;}
  default:
    free(field_str);
    break;
  }

  if(global_field_count != -1)
    global_field_count ++;
  return;
}

void end_of_row_processor(int last_char, void *output){
  // reset the global variables
  if(global_field_count == -1){
    printf("Warning: Invalid function row in file. \n");
  }
  else if(global_field_count != 8){
    debug_printf("Error: %s\n", "need to remove the incomplete lines first.");
    exit(EXIT_FAILURE);
  }
  else{
    Vector *parameter_arr = output;
    // normally, last_char should be '\n', not useful here
    if(!in_parameter_arr){
      printf("Add a parameter: %s\n", current_parameter->name);
      vector_append(parameter_arr, current_parameter);
    }
  }
  
  global_field_count = 0;
  in_parameter_arr = 0;
  current_parameter = 0;
}

// Utility functions - definition
int is_csv_file(const char *path)
{
  struct stat path_stat;
  stat(path, &path_stat);

  /* debugging use: to print the type of the input path
  switch (path_stat.st_mode & S_IFMT) {
  case S_IFBLK:  printf("block device\n");            break;
  case S_IFCHR:  printf("character device\n");        break;
  case S_IFDIR:  printf("directory\n");               break;
  case S_IFIFO:  printf("FIFO/pipe\n");               break;
  case S_IFLNK:  printf("symlink\n");                 break;
  case S_IFSOCK: printf("socket\n");                  break;
  case S_IFREG:  printf("regular file\n");            break;
  default:       printf("unknown?\n");                break;
  }*/

  switch (path_stat.st_mode & S_IFMT) {
  case S_IFBLK:  
  case S_IFCHR:  
  case S_IFDIR:  
  case S_IFIFO:  
  case S_IFLNK:  
  case S_IFSOCK:
    return 0;
  case S_IFREG:
  default:
    if(ends_with(path, ".csv"))
      return 1;
    else
      return 0;
  }
}

int is_subfolder(const char *path){
  struct stat path_stat;
  stat(path, &path_stat);    
  if (S_ISDIR(path_stat.st_mode)){
    return 1;
  }
  return 0;
}

int is_headline(const char *line){
  int starts_with_ret = starts_with(line, "Function,") ||  starts_with(line, "Is Return Flag,");
  return starts_with_ret;
}

DIR *open_input_directory(const char *dir){
  // check whether the input directory exists
  struct stat st = {0};  
  if(stat(dir, &st) == -1){
    fprintf(stderr, "Cannot find the input directory: %s \n", dir);
    printf("%s", help);
    exit(EXIT_FAILURE);
  }
  
  // open the input directory
  DIR *d = opendir(dir);
  if (!d){
    fprintf(stderr, "Cannot open the input directory: %s \n", dir);
    fprintf(stderr, "ERROR: %s\n", strerror(errno));
    printf("%s", help);
    exit(EXIT_FAILURE);
  }
  return d;
}

int get_function_name_and_real_path(struct dirent *dir_ent, const char* parent_dir,
				char *output_function_name, char *output_real_path){
  if(strcmp(dir_ent->d_name, ".")== 0 || strcmp(dir_ent->d_name, "..")== 0)
     return 0;

  // Get the function name
  strcpy(output_function_name, dir_ent->d_name);
  
  // Get the file name and real path of the file
  char output_file_name[PATH_MAX+1];
  strcpy(output_file_name, parent_dir);
  if(!ends_with(parent_dir, "/"))
    strcat(output_file_name, "/");
  strcat(output_file_name, dir_ent->d_name);
  char *realpath_ret = realpath(output_file_name, output_real_path);
  if(realpath_ret == NULL){
    fprintf(stderr, "Cannot get the real path of: %s \n", output_file_name);
    fprintf(stderr, "ERROR: %s\n", strerror(errno));
    return 0; 
  }
  return 1;
}

int get_sub_name_and_real_path(struct dirent *dir_ent, const char* parent_dir,
				char *output_file_name, char *output_real_path){
  if(strcmp(dir_ent->d_name, ".")== 0 || strcmp(dir_ent->d_name, "..")== 0)
     return 0;
  
  // Get the file name and real path of the file
  strcpy(output_file_name, parent_dir);
  if(!ends_with(parent_dir, "/"))
    strcat(output_file_name, "/");
  strcat(output_file_name, dir_ent->d_name);
  char *realpath_ret = realpath(output_file_name, output_real_path);
  if(realpath_ret == NULL){
    fprintf(stderr, "Cannot get the real path of: %s \n", output_file_name);
    fprintf(stderr, "ERROR: %s\n", strerror(errno));
    return 0; 
  }
  return 1;
}

int try_open_file_and_get_first_line(const char *file_name, const char *file_real_path, char **line_p, int *read_char_number, FILE **fpp){
  // try to open the file
  FILE * fp;
  char * line = NULL;
  size_t len = 0;
  *read_char_number = 0;
  
  fp = fopen(file_name, "r");
  if (fp == NULL){
    fprintf(stderr, "Cannot open the file: [%s] \n", file_name);
    fprintf(stderr, "           real path: [%s] \n", file_real_path);
    fprintf(stderr, "ERROR: %s\n", strerror(errno));
    return -1;
  }
  
  // read the first line, check if there is a line
  if((*read_char_number = getline(&line, &len, fp)) == -1){
    fprintf(stderr, "No line in file: [%s]\n", file_real_path);
    if(line) free(line); // free the buffer that getline creates
    fclose(fp); // close the file
    return -1;
  }
  
  // if there is a line, check if the first line is a headline
  int is_headline_ret = is_headline(line);
  if(is_headline_ret == 1){
    // if this is a head line, then we skip this line, and read the second line
    if((*read_char_number = getline(&line, &len, fp)) == -1){
      fprintf(stderr, "No line except a headline in file: [%s]\n", file_real_path);
      if(line) free(line); // free the buffer that getline creates
      fclose(fp); // close the file
      return -1;
    }
  }
  
  *fpp = fp;
  *line_p = line;
  return 0;
}

void global_variables_init(const char *file_name){
  global_field_count = 0;
  return_flag = 0;
  current_parameter = 0;
  in_parameter_arr = 0;
  previous_call_id = -1;
  current_run_name = file_name;
}

void printf_parameter_ids(Vector parameter_arr, FILE *output_file){
  // print the parameters
  printf("parameter arr: %d\n", parameter_arr.size);
  for(int i=0; i<parameter_arr.size; i++){
    int current_parameter_id = i;
    struct parameter_entry* p = vector_get(&parameter_arr, i);
    if(parameter_can_be_null(*p))
      fprintf(output_file, "%d\t%d\t%s\t%s\t%s\t\%s\n", current_function_id, current_parameter_id, current_function_name, p->type, p->name, "can be null");
    else
      fprintf(output_file, "%d\t%d\t%s\t%s\t%s\t\%s\n", current_function_id, current_parameter_id, current_function_name, p->type, p->name, "do not know");
  }

  /*
  for(int i=0; i<parameter_arr.size; i++){
    struct parameter_entry* p = vector_get(&parameter_arr, i);
    printf("%s\t%s\n", p->type, p->name);
    printf("input: \n");
    printf_value_list(p->input_values);
    printf("output: \n");
    printf_value_list(p->output_values);
    printf("changed: \n");
    printf_changed_value_list(p->changed_values);
    }*/
}

void printf_io_examples(Vector parameter_arr, FILE* output_file){
  for(int i=0; i<parameter_arr.size; i++){
    int current_parameter_id = i;
    struct parameter_entry* p = vector_get(&parameter_arr, i);
    parameter_printf_random_value(p, output_file);
  }
  return;
}

void copy_head_from_file(const char *source_file, const char*dest_file, int end_line_number){
  printf("copy to line: %d\n", end_line_number);
  FILE *source_fp = NULL;
  char *line = NULL;
  int read_char_number = 0;
  int ret = try_open_file_and_get_first_line(source_file, source_file, &line, &read_char_number, &source_fp);
  if(ret == -1) return -1;

  FILE *dest_fp = NULL;
  dest_fp = fopen (dest_file, "w" );
  
  size_t len = strlen(line);
  int line_number = 0;
  do{
    if(len < 3) continue;
    char *new_line = malloc(sizeof(char) * (len + 2));
    strcpy(new_line, line);
    make_sure_new_line_ends(new_line);
    if(line_number > end_line_number)
      break;
    fprintf(dest_fp, "%s", new_line);
    free(new_line);
    line_number ++;
  } // end loop: read the file line by line
  while ((read_char_number = getline(&line, &len, source_fp)) != -1);
  if (line) // free the buffer that getline creates
    free(line);
  fclose(source_fp); // close the file
  fclose(dest_fp); // close the file
  return;
}

void make_sure_new_line_ends(char *line){
  // input line should have three more bytes than len
  size_t len = strlen(line);
  
  if(line[len] != '\0'){
    debug_printf("Error: %s\n", "the end of string should be a \\0");
  }else{
    if(line[len -1] != '\n'){
      line[len] = '\n';
      line[len + 1] = '\0';
    }
  }
}
