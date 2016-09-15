/*
 * 
 */

#include <stdio.h>
#include <unistd.h>

FILE * fp;

int foo(int **parameter_1){
  fprintf(fp, "foo, 0, parameter_1, %zu, ?, int *, %d\n", sizeof(parameter_1), *parameter_1);
  int *p = *parameter_1;
  int v = *p;
  int ret =  v+1;
  return ret;
}

int main(int argc, char *argv[]) {
  int tmp = -1;
  int *tmp2 = &tmp;
  int **arg1 = &tmp2;
  
  fp = fopen ("expected_csv.csv", "w");
  fprintf(fp, "Function, Call Number, Variable Name, Variable Size, Usage Flags, Variable Type, Value \n");

  int ret = foo(arg1);
  
  fprintf(fp, "foo, 0, $return_value, %zu, ?, int, %d\n", sizeof(int), ret);
  fclose(fp);
  
  return 0;
}

