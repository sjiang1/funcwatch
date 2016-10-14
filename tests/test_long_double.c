/*                                                                                                                            * To test funcwatch, run funcwatch against foo.                                                                              * Compare funcwatch's result with the newly generated expected_csv.csv                                                       */

#include <stdio.h>
#include <unistd.h>

long double foo(long double parameter_1){
  return parameter_1;
}

int main(int argc, char *argv[]) {
  long double arg1 = 3.45;

  long double ret = foo(arg1);
  return 0;
}
