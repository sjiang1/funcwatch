/*
 * This file tests all the primitive types for a struct member.
 * To test funcwatch, run funcwatch against foo. 
 * Compare funcwatch's result with the newly generated expected_csv.csv
 */

#include <stdio.h>
#include <unistd.h>

FILE * fp;

int foo(int *parameter_1){
  //printf("Parameter address: %p\n", &parameter_1);
  fprintf(fp, "foo, 0, parameter_1, %zu, ?, int *, %d\n", sizeof(parameter_1), *parameter_1);
  *parameter_1 = 10;
  return parameter_1;
}

int main(int argc, char *argv[]) {
  int tmp = -1;
  int *arg1 = &tmp;
  
  fp = fopen ("expected_csv.csv", "w");
  fprintf(fp, "Function, Call Number, Variable Name, Variable Size, Usage Flags, Variable Type, Value \n");

  int ret = foo(arg1);
  
  fprintf(fp, "foo, 0, $return_value, %zu, ?, int, %d\n", sizeof(int), ret);
  fclose(fp);
  
  return 0;
}

