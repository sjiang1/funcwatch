/*
 * This file tests all the primitive types for a struct member.
 * To test funcwatch, run funcwatch against foo. 
 * Compare funcwatch's result with the newly generated expected_csv.csv
 */

#include <stdio.h>
#include <unistd.h>

union my_union_1{
  int member_1;
  char member_2;
  signed char member_3;
  unsigned char member_4;
  short member_5;
  short int member_6;
  signed short member_7;
  signed short int member_8;
  unsigned short int member_9;
  signed member_10;
  signed int member_11;
  unsigned member_12;
  unsigned int member_13;
  long member_14;
  long int member_15;
  signed long member_16;
  signed long int member_17;
  unsigned long member_18;
  unsigned long int member_19;
  long long member_20;
  long long int member_21;
  signed long long member_22;
  signed long long int member_23;
  unsigned long long member_24;
  unsigned long long int member_25;
  float member_26;
  double member_27;
  long double member_28;
};

FILE * fp;

int foo(union my_union_1 parameter_1){
  fprintf(fp, "foo, 0, parameter_1, %zu, ?, union my_union_1, %p\n", sizeof(parameter_1), &parameter_1);
  fprintf(fp, "foo, 0, parameter_1.member_2, %zu, ?, char, %c\n", sizeof(char), parameter_1.member_2);
  return parameter_1.member_1;
}

int main(int argc, char *argv[]) {
  union my_union_1 arg1;
  arg1.member_2 = 2+255;
  
  fp = fopen ("expected_csv.csv", "w");
  fprintf(fp, "Function, Call Number, Variable Name, Variable Size, Usage Flags, Variable Type, Value \n");

  int ret = foo(arg1);
  
  fprintf(fp, "foo, 0, $return_value, %zu, ?, int, %d\n", sizeof(int), ret);
  fclose(fp);
  
  return 0;
}

