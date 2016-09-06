/*
 * This file tests function to log a recursive function's IO
 * To test funcwatch, run funcwatch against foo. 
 * Compare funcwatch's result with the newly generated expected_csv.csv
 */

#include <stdio.h>
#include <unistd.h>

FILE * fp;

int foo(int i){
  if (i > 0){
    i = i -1;
    foo(i);
    return i;
  }
  return 10;
}

int main(int argc, char *argv[]) {
  int i = 2;
  int ret = foo(i);

  fp = fopen ("expected_csv.csv", "w");
  fprintf(fp, "Function, Call Number, Variable Name, Variable Size, Usage Flags, Variable Type, Value \n");
  fprintf(fp, "foo, 0, i, %zu, ?, int, %d\n", sizeof(i), 2);
  fprintf(fp, "foo, 0, $return_value, %zu, ?, unsigned char, %hhu\n", sizeof(ret), 1);
  
  fprintf(fp, "foo, 1, i, %zu, ?, int, %d\n", sizeof(i), 1);
  fprintf(fp, "foo, 1, $return_value, %zu, ?, unsigned char, %hhu\n", sizeof(ret), 0);

  fprintf(fp, "foo, 2, i, %zu, ?, int, %d\n", sizeof(i), 0);
  fprintf(fp, "foo, 2, $return_value, %zu, ?, unsigned char, %hhu\n", sizeof(ret), 10);
  fclose(fp);
  
  return 0;
}

