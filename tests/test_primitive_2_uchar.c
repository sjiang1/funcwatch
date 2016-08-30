/*
 * This file tests all the primitive types for a struct member.
 * To test funcwatch, run funcwatch against foo. 
 * Compare funcwatch's result with the newly generated expected_csv.csv
 */

#include <stdio.h>
#include <unistd.h>

FILE * fp;

unsigned char foo(unsigned char parameter_1){
  printf("Parameter address: %p\n", &parameter_1);
  printf("Parameter bit value:");
  unsigned char tmpvalue = parameter_1;
  while (tmpvalue) {
    if (tmpvalue & 1)
      printf("1");
    else
      printf("0");

    tmpvalue >>= 1;
  }
  printf("\n");
  
  fprintf(fp, "foo, 0, parameter_1, %zu, ?, unsigned char, %hhu\n", sizeof(parameter_1), parameter_1);
  return parameter_1;
}

int main(int argc, char *argv[]) {
  unsigned char arg1 = -1;
  
  fp = fopen ("expected_csv.csv", "w");
  fprintf(fp, "Function, Call Number, Variable Name, Variable Size, Usage Flags, Variable Type, Value \n");

  unsigned char ret = foo(arg1);
  
  fprintf(fp, "foo, 0, $return_value, %zu, ?, unsigned char, %hhu\n", sizeof(ret), ret);
  fclose(fp);
  
  return 0;
}

