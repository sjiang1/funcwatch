/*
 * This file tests all the primitive types for a struct member.
 * To test funcwatch, run funcwatch against foo. 
 * Compare funcwatch's result with the newly generated expected_csv.csv
 */

#include <stdio.h>
#include <unistd.h>

union my_union_1{
  int member_1;
  char *member_2;
};

FILE * fp;

int foo(union my_union_1 parameter_1){
  fprintf(fp, "foo, 0, parameter_1, %zu, ?, union my_union_1, [memory address]\n", sizeof(parameter_1));
  fprintf(fp, "foo, 0, parameter_1.member_1, %zu, ?, int, %d\n", sizeof(int), parameter_1.member_1);
  fprintf(fp, "foo, 0, parameter_1.member_2, %zu, ?, char *, [invalid]\n", 0);
  return parameter_1.member_1;
}

int main(int argc, char *argv[]) {
  union my_union_1 arg1;
  arg1.member_1 = 2+255;
  
  fp = fopen ("expected_csv.csv", "w");
  fprintf(fp, "Function, Call Number, Variable Name, Variable Size, Usage Flags, Variable Type, Value \n");

  int ret = foo(arg1);
  
  fprintf(fp, "foo, 0, $return_value, %zu, ?, int, %d\n", sizeof(int), ret);
  fclose(fp);
  
  return 0;
}

