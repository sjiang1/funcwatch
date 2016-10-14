/*
 * This file tests all the primitive types for a struct member.
 * To test funcwatch, run funcwatch against foo. 
 * Compare funcwatch's result with the newly generated expected_csv.csv
 */

#include <stdio.h>
#include <unistd.h>

struct struct_type{
  struct my_struct *member_1;
};

struct my_struct{
  int a;
};

int foo(struct struct_type **parameter_1){
  parameter_1[0]->member_1->a = 10;
  return parameter_1[0]->member_1->a;
}

int main(int argc, char *argv[]) {
  struct struct_type tmp1;
  struct struct_type *arg1[1];
  arg1[0] = &tmp1;
  struct my_struct tmp2;
  arg1[0]->member_1 = &tmp2;
  
  arg1[0]->member_1->a = -1;
  
  int ret = foo(arg1);
  
  return 0;
}

