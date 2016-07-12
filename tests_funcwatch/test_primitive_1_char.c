/*
 * This file tests all the primitive types for a struct member.
 * To test funcwatch, run funcwatch against foo. 
 * Compare funcwatch's result with the newly generated expected_csv.csv
 */

#include <stdio.h>
#include <unistd.h>

FILE * fp;

char foo1(char parameter_1){
  //printf("Parameter address: %p\n", &parameter_1);
  //fprintf(fp, "foo, 0, parameter_1, %zu, ?, char, %c\n", sizeof(parameter_1), parameter_1);
  return parameter_1;
}

char foo(char parameter_1){
  printf("Parameter address: %p\n", &parameter_1);
  //fprintf(fp, "foo, 0, parameter_1, %zu, ?, char, %c\n", sizeof(parameter_1), parameter_1);
  //parameter_1 --;
  //parameter_1 = foo1(parameter_1);
  //parameter_1 --;
  return parameter_1;
}

int main(int argc, char *argv[]) {
  char arg1 = 'x';
  
  fp = fopen ("expected_csv.csv", "w");
  fprintf(fp, "Function, Call Number, Variable Name, Variable Size, Usage Flags, Variable Type, Value \n");

  char ret = foo(arg1);
  
  fprintf(fp, "foo, 0, $return_value, %zu, ?, char, %c\n", sizeof(int), ret);
  fclose(fp);
  
  return 0;
}

