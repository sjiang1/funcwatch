/*
 * This file tests all the primitive types for a struct member.
 * To test funcwatch, run funcwatch against foo. 
 * Compare funcwatch's result with the newly generated expected_csv.csv
 */

#include <stdio.h>
#include <unistd.h>

FILE * fp;

int foo(int parameter_1, int parameter_2, int parameter_3){
  fprintf(fp, "foo, 0, parameter_1, %zu, ?, int, %d\n", sizeof(parameter_1), parameter_1);
  fprintf(fp, "foo, 0, parameter_2, %zu, ?, int, %d\n", sizeof(parameter_2), parameter_2);
  fprintf(fp, "foo, 0, parameter_2, %zu, ?, int, %d\n", sizeof(parameter_3), parameter_3);
  return parameter_1;
}

int main(int argc, char *argv[]) {
  int arg1 = -1;
  int arg2 = -2;
  int arg3 = 2;
  
  fp = fopen ("expected_csv.csv", "w");
  fprintf(fp, "Function, Call Number, Variable Name, Variable Size, Usage Flags, Variable Type, Value \n");

  int ret = foo(arg1, arg2, arg3);
  
  fprintf(fp, "foo, 0, $return_value, %zu, ?, unsigned char, %d\n", sizeof(ret), ret);
  fclose(fp);
  
  return 0;
}

