/*
 * This file tests all the primitive types for a struct member.
 * To test funcwatch, run funcwatch against foo. 
 * Compare funcwatch's result with the newly generated expected_csv.csv
 */

#include <stdio.h>
#include <unistd.h>

struct my_struct_1{
  int *member_1;
  char *member_2;
  signed char *member_3;
  unsigned char *member_4;
  short *member_5;
  short int *member_6;
  signed short *member_7;
  signed short int *member_8;
  unsigned short int *member_9;
  signed *member_10;
  signed int *member_11;
  unsigned *member_12;
  unsigned int *member_13;
  long *member_14;
  long int *member_15;
  signed long *member_16;
  signed long int *member_17;
  unsigned long *member_18;
  unsigned long int *member_19;
  long long *member_20;
  long long int *member_21;
  signed long long *member_22;
  signed long long int *member_23;
  unsigned long long *member_24;
  unsigned long long int *member_25;
  float *member_26;
  double *member_27;
  long double *member_28;
  void *member_29;
};

FILE * fp;

int foo(struct my_struct_1 parameter_1){
  fprintf(fp, "foo, 0, parameter_1, %zu, ?, struct my_struct_1, %p\n", sizeof(parameter_1), &parameter_1);
  fprintf(fp, "foo, 0, parameter_1.member_1, %zu, ?, int *, %d\n", sizeof(parameter_1.member_1), *parameter_1.member_1);
  fprintf(fp, "foo, 0, parameter_1.member_2, %zu, ?, char *, %c\n", sizeof(parameter_1.member_1), *parameter_1.member_2);
  fprintf(fp, "foo, 0, parameter_1.member_3, %zu, ?, signed char *, %hhd\n", sizeof(parameter_1.member_1), *parameter_1.member_3);
  fprintf(fp, "foo, 0, parameter_1.member_4, %zu, ?, unsigned char *, %hhu\n", sizeof(parameter_1.member_1), *parameter_1.member_4);
  fprintf(fp, "foo, 0, parameter_1.member_5, %zu, ?, short *, %hd\n", sizeof(parameter_1.member_1), *parameter_1.member_5);
  fprintf(fp, "foo, 0, parameter_1.member_6, %zu, ?, short int *, %hd\n", sizeof(parameter_1.member_1), *parameter_1.member_6);
  fprintf(fp, "foo, 0, parameter_1.member_7, %zu, ?, signed short *, %hd\n", sizeof(parameter_1.member_1), *parameter_1.member_7);
  fprintf(fp, "foo, 0, parameter_1.member_8, %zu, ?, signed short int *, %hd\n", sizeof(parameter_1.member_1), *parameter_1.member_8);
  fprintf(fp, "foo, 0, parameter_1.member_9, %zu, ?, unsigned short int *, %hu\n", sizeof(parameter_1.member_1), *parameter_1.member_9);
  fprintf(fp, "foo, 0, parameter_1.member_10, %zu, ?, signed *, %d\n", sizeof(parameter_1.member_1), *parameter_1.member_10);
  fprintf(fp, "foo, 0, parameter_1.member_11, %zu, ?, signed int *, %d\n", sizeof(parameter_1.member_1), *parameter_1.member_11);
  fprintf(fp, "foo, 0, parameter_1.member_12, %zu, ?, unsigned *, %u\n", sizeof(parameter_1.member_1), *parameter_1.member_12);
  fprintf(fp, "foo, 0, parameter_1.member_13, %zu, ?, unsigend int *, %u\n", sizeof(parameter_1.member_1), *parameter_1.member_13);
  fprintf(fp, "foo, 0, parameter_1.member_14, %zu, ?, long *, %ld\n", sizeof(parameter_1.member_1), *parameter_1.member_14);
  fprintf(fp, "foo, 0, parameter_1.member_15, %zu, ?, long int *, %ld\n", sizeof(parameter_1.member_1), *parameter_1.member_15);
  fprintf(fp, "foo, 0, parameter_1.member_16, %zu, ?, signed long *, %ld\n", sizeof(parameter_1.member_1), *parameter_1.member_16);
  fprintf(fp, "foo, 0, parameter_1.member_17, %zu, ?, signed long int *, %ld\n", sizeof(parameter_1.member_1), *parameter_1.member_17);
  fprintf(fp, "foo, 0, parameter_1.member_18, %zu, ?, unsigned long *, %lu\n", sizeof(parameter_1.member_1), *parameter_1.member_18);
  fprintf(fp, "foo, 0, parameter_1.member_19, %zu, ?, unsigned long int *, %lu\n", sizeof(parameter_1.member_1), *parameter_1.member_19);
  fprintf(fp, "foo, 0, parameter_1.member_20, %zu, ?, long long *, %lld\n", sizeof(parameter_1.member_1), *parameter_1.member_20);
  fprintf(fp, "foo, 0, parameter_1.member_21, %zu, ?, long long int *, %lld\n", sizeof(parameter_1.member_1),*parameter_1.member_21);
  fprintf(fp, "foo, 0, parameter_1.member_22, %zu, ?, signed long long *, %lld\n", sizeof(parameter_1.member_1), *parameter_1.member_22);
  fprintf(fp, "foo, 0, parameter_1.member_23, %zu, ?, signed long long int *, %lld\n", sizeof(parameter_1.member_1), *parameter_1.member_23);
  fprintf(fp, "foo, 0, parameter_1.member_24, %zu, ?, unsigned long long *, %llu\n", sizeof(parameter_1.member_1), *parameter_1.member_24);
  fprintf(fp, "foo, 0, parameter_1.member_25, %zu, ?, unsigned long long int *, %llu\n", sizeof(parameter_1.member_1), *parameter_1.member_25);
  fprintf(fp, "foo, 0, parameter_1.member_26, %zu, ?, float *, %f\n", sizeof(parameter_1.member_1), *parameter_1.member_26);
  fprintf(fp, "foo, 0, parameter_1.member_27, %zu, ?, double *, %lf\n", sizeof(parameter_1.member_1), *parameter_1.member_27);
  fprintf(fp, "foo, 0, parameter_1.member_28, %zu, ?, long double *, %LF\n", sizeof(parameter_1.member_1), *parameter_1.member_28);
  fprintf(fp, "foo, 0, parameter_1.member_29, %zu, ?, void *, %p\n", sizeof(parameter_1.member_29), parameter_1.member_29);
  return parameter_1.member_1;
}

int main(int argc, char *argv[]) {
  int member_1 = 1+2147483647;
  char member_2 = 257;
  signed char member_3 = 258;
  unsigned char member_4 = -1;
  short member_5 = -2;
  short int member_6 = -3;
  signed short member_7 = -4;
  signed short int member_8 = -5;
  unsigned short int member_9 = -6;
  signed member_10 = -7;
  signed int member_11 = -8;
  unsigned member_12 = -9;
  unsigned int member_13 = -10;
  long member_14 = -11;
  long int member_15 = -12;
  signed long member_16 = -13;
  signed long int member_17 = -14;
  unsigned long member_18 = -15;
  unsigned long int member_19 = -16;
  long long member_20 = -17;
  long long int member_21 = -18;
  signed long long member_22 = -19;
  signed long long int member_23 = -20;
  unsigned long long member_24 = -21;
  unsigned long long int member_25 = -22;
  float member_26 = 3.4;//E+39;
  double member_27 = 1.7;//E+308+1;
  long double member_28 = 3.4;//E-4932;

  struct my_struct_1 arg1;
  
  arg1.member_1 = &member_1; 
  arg1.member_2 = &member_2; 
  arg1.member_3 = &member_3; 
  arg1.member_4 = &member_4; 
  arg1.member_5 = &member_5; 
  arg1.member_6 = &member_6; 
  arg1.member_7 = &member_7; 
  arg1.member_8 = &member_8; 
  arg1.member_9 = &member_9; 
  arg1.member_10 = &member_10;
  arg1.member_11 = &member_11;
  arg1.member_12 = &member_12;
  arg1.member_13 = &member_13;
  arg1.member_14 = &member_14;
  arg1.member_15 = &member_15;
  arg1.member_16 = &member_16;
  arg1.member_17 = &member_17;
  arg1.member_18 = &member_18;
  arg1.member_19 = &member_19;
  arg1.member_20 = &member_20;
  arg1.member_21 = &member_21;
  arg1.member_22 = &member_22;
  arg1.member_23 = &member_23;
  arg1.member_24 = &member_24;
  arg1.member_25 = &member_25;
  arg1.member_26 = &member_26;
  arg1.member_27 = &member_27;
  arg1.member_28 = &member_28;
  arg1.member_29 = "abc";
    
  fp = fopen ("expected_csv.csv", "w");
  fprintf(fp, "Function, Call Number, Variable Name, Variable Size, Usage Flags, Variable Type, Value \n");

  int ret = foo(arg1);
  
  fprintf(fp, "foo, 0, $return_value, %zu, ?, int, %d\n", sizeof(int), ret);
  fclose(fp);
  
  return 0;
}

