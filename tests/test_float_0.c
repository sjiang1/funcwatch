/*
 * This file tests funcwatch collecting float-type return value
 * To test funcwatch, run funcwatch against foo. 
 */

#include <stdio.h>
#include <unistd.h>

float foo(int parameter_1){
  float a = parameter_1 * 0.001;
  return a;
}

void main(int argc, char *argv[]) {
  int arg1 = -1;
  
  float ret = foo(arg1);
}

