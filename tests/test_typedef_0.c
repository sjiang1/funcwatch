/*
 * To test funcwatch, run funcwatch against foo.
 * Compare funcwatch's result with the newly generated expected_csv.csv
 */

#include <stdio.h>
#include <unistd.h>

typedef int MYINT;

int foo(MYINT parameter_1){
  return parameter_1;
}

int main(int argc, char *argv[]) {
  MYINT arg1 = -1;

  int ret = foo(arg1);
  return 0;
}

