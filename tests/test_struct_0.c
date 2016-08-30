/*
 * This file tests all the primitive types for a struct member.
 * To test funcwatch, run funcwatch against foo. 
 * Compare funcwatch's result with the newly generated expected_csv.csv
 */

#include <stdio.h>
#include <unistd.h>

struct my_struct_1{
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

int foo(struct my_struct_1 parameter_1){
  fprintf(fp, "foo, 0, parameter_1, %zu, ?, struct my_struct_1, %p\n", sizeof(parameter_1), &parameter_1);
  fprintf(fp, "foo, 0, parameter_1.member_1, %zu, ?, int, %d\n", sizeof(int), parameter_1.member_1);
  fprintf(fp, "foo, 0, parameter_1.member_2, %zu, ?, char, %c\n", sizeof(char), parameter_1.member_2);
  fprintf(fp, "foo, 0, parameter_1.member_3, %zu, ?, signed char, %hhd\n", sizeof(signed char), parameter_1.member_3);
  fprintf(fp, "foo, 0, parameter_1.member_4, %zu, ?, unsigned char, %hhu\n", sizeof(unsigned char), parameter_1.member_4);
  fprintf(fp, "foo, 0, parameter_1.member_5, %zu, ?, short, %hd\n", sizeof(short), parameter_1.member_5);
  fprintf(fp, "foo, 0, parameter_1.member_6, %zu, ?, short int, %hd\n", sizeof(short int), parameter_1.member_6);
  fprintf(fp, "foo, 0, parameter_1.member_7, %zu, ?, signed short, %hd\n", sizeof(signed short), parameter_1.member_7);
  fprintf(fp, "foo, 0, parameter_1.member_8, %zu, ?, signed short int, %hd\n", sizeof(signed short int), parameter_1.member_8);
  fprintf(fp, "foo, 0, parameter_1.member_9, %zu, ?, unsigned short in, %hu\n", sizeof(unsigned short int), parameter_1.member_9);
  fprintf(fp, "foo, 0, parameter_1.member_10, %zu, ?, signed, %d\n", sizeof(signed), parameter_1.member_10);
  fprintf(fp, "foo, 0, parameter_1.member_11, %zu, ?, signed int, %d\n", sizeof(signed int), parameter_1.member_11);
  fprintf(fp, "foo, 0, parameter_1.member_12, %zu, ?, unsigned, %u\n", sizeof(unsigned), parameter_1.member_12);
  fprintf(fp, "foo, 0, parameter_1.member_13, %zu, ?, unsigend int, %u\n", sizeof(unsigned int), parameter_1.member_13);
  fprintf(fp, "foo, 0, parameter_1.member_14, %zu, ?, long, %ld\n", sizeof(long), parameter_1.member_14);
  fprintf(fp, "foo, 0, parameter_1.member_15, %zu, ?, long int, %ld\n", sizeof(long int), parameter_1.member_15);
  fprintf(fp, "foo, 0, parameter_1.member_16, %zu, ?, signed long, %ld\n", sizeof(signed long), parameter_1.member_16);
  fprintf(fp, "foo, 0, parameter_1.member_17, %zu, ?, signed long int, %ld\n", sizeof(signed long int), parameter_1.member_17);
  fprintf(fp, "foo, 0, parameter_1.member_18, %zu, ?, unsigned long, %lu\n", sizeof(unsigned long), parameter_1.member_18);
  fprintf(fp, "foo, 0, parameter_1.member_19, %zu, ?, unsigned long int, %lu\n", sizeof(unsigned long int), parameter_1.member_19);
  fprintf(fp, "foo, 0, parameter_1.member_20, %zu, ?, long long, %lld\n", sizeof(long long), parameter_1.member_20);
  fprintf(fp, "foo, 0, parameter_1.member_21, %zu, ?, long long int, %lld\n", sizeof(long long int), parameter_1.member_21);
  fprintf(fp, "foo, 0, parameter_1.member_22, %zu, ?, signed long long, %lld\n", sizeof(signed long long), parameter_1.member_22);
  fprintf(fp, "foo, 0, parameter_1.member_23, %zu, ?, signed long long int, %lld\n", sizeof(signed long long int), parameter_1.member_23);
  fprintf(fp, "foo, 0, parameter_1.member_24, %zu, ?, unsigned long long, %llu\n", sizeof(unsigned long long), parameter_1.member_24);
  fprintf(fp, "foo, 0, parameter_1.member_25, %zu, ?, unsigned long long int, %llu\n", sizeof(unsigned long long int), parameter_1.member_25);
  fprintf(fp, "foo, 0, parameter_1.member_26, %zu, ?, float, %f\n", sizeof(float), parameter_1.member_26);
  fprintf(fp, "foo, 0, parameter_1.member_27, %zu, ?, double, %lf\n", sizeof(double), parameter_1.member_27);
  fprintf(fp, "foo, 0, parameter_1.member_28, %zu, ?, long double, %LF\n", sizeof(long double), parameter_1.member_28);
  return parameter_1.member_1;
}

int main(int argc, char *argv[]) {
  struct my_struct_1 arg1;
  arg1.member_1 = 1+2147483647;
  arg1.member_2 = 2+255;
  arg1.member_3 = 3+255;
  arg1.member_4 = -1;
  arg1.member_5 = 5+30767;
  arg1.member_6 = 6+30767;
  arg1.member_7 = 7+30767;
  arg1.member_8 = 8+30767;
  arg1.member_9 = 9+30767;
  arg1.member_10 = 10+2147483647;
  arg1.member_11 = 11+2147483647;
  arg1.member_12 = 12+2147483647;
  arg1.member_13 = 13+2147483647;
  arg1.member_14 = 14+2147483647;
  arg1.member_15 = 15+2147483647;
  arg1.member_16 = 16+2147483647;
  arg1.member_17 = 17+2147483647;
  arg1.member_18 = 18+2147483647;
  arg1.member_19 = 19+2147483647;
  arg1.member_20 = -1;
  arg1.member_21 = -1;
  arg1.member_22 = -1;
  arg1.member_23 = -1;
  arg1.member_24 = 1;
  arg1.member_25 = 1;
  arg1.member_26 = 3.4;//E+39;
  arg1.member_27 = 1.7;//E+308+1;
  arg1.member_28 = 3.4;//E-4932;
  
  fp = fopen ("expected_csv.csv", "w");
  fprintf(fp, "Function, Call Number, Variable Name, Variable Size, Usage Flags, Variable Type, Value \n");

  int ret = foo(arg1);
  
  fprintf(fp, "foo, 0, $return_value, %zu, ?, int, %d\n", sizeof(int), ret);
  fclose(fp);
  
  return 0;
}

