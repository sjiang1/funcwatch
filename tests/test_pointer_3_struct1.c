/*
 * This file tests all the primitive types for a struct member.
 * To test funcwatch, run funcwatch against foo. 
 * Compare funcwatch's result with the newly generated expected_csv.csv
 */

#include <stdio.h>
#include <unistd.h>

FILE * fp;

struct struct_type{
  struct my_struct *member_1;
};

struct my_struct{
  int a;
};

int foo(struct struct_type *parameter_1){
  fprintf(fp, "foo, 0, parameter_1, %zu, ?, struct struct_type *, %p\n", sizeof(parameter_1), parameter_1);
  fprintf(fp, "foo, 0, parameter_1.a, %zu, ?, struct my_struct, %p\n", sizeof(parameter_1->member_1), parameter_1->member_1);
  fprintf(fp, "foo, 0, parameter_1.b, %zu, ?, int, %d\n", sizeof(parameter_1->member_1->a), parameter_1->member_1->a);
  return parameter_1->member_1->a;
}

int main(int argc, char *argv[]) {
  struct struct_type tmp1;
  struct struct_type *arg1 = &tmp1;
  struct my_struct tmp2;
  arg1->member_1 = &tmp2;
  
  arg1->member_1->a = -1;
  
  fp = fopen ("expected_csv.csv", "w");
  fprintf(fp, "Function, Call Number, Variable Name, Variable Size, Usage Flags, Variable Type, Value \n");

  int ret = foo(arg1);
  
  fprintf(fp, "foo, 0, $return_value, %zu, ?, int, %d\n", sizeof(ret), ret);
  fclose(fp);
  
  return 0;
}

