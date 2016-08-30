/*
 * This file tests all the primitive types for a struct member.
 * To test funcwatch, run funcwatch against foo. 
 * Compare funcwatch's result with the newly generated expected_csv.csv
 */

#include <stdio.h>
#include <unistd.h>

struct my_struct_1{
  int member_1;
};

struct my_struct_2{
  struct my_struct_1 *member_1;
};

FILE * fp;

int foo(struct my_struct_2 parameter_1){
  fprintf(fp, "foo, 0, parameter_1, %zu, ?, struct my_struct_2, %p\n", sizeof(parameter_1), &parameter_1);
  fprintf(fp, "foo, 0, parameter_1.member_1, %zu, ?, my_struct_1 *, %p\n", sizeof(parameter_1.member_1), parameter_1.member_1);
  fprintf(fp, "foo, 0, parameter_1.member_1.member1, %zu, ?, int, %d\n", sizeof(parameter_1.member_1->member_1),
	  parameter_1.member_1->member_1);
  return parameter_1.member_1->member_1;
}

int main(int argc, char *argv[]) {
  struct my_struct_1 tmp;
  struct my_struct_2 arg1;
  arg1.member_1 = &tmp;
  tmp.member_1 = -1;
  
  fp = fopen ("expected_csv.csv", "w");
  fprintf(fp, "Function, Call Number, Variable Name, Variable Size, Usage Flags, Variable Type, Value \n");

  int ret = foo(arg1);
  
  fprintf(fp, "foo, 0, $return_value, %zu, ?, int, %d\n", sizeof(int), ret);
  fclose(fp);
  
  return 0;
}

