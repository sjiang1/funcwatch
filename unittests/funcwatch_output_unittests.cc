#include "../funcwatch_output.h"
#include "../funcwatch_param_util.h"
#include "../dynstring.h"
#include "gtest/gtest.h"

TEST(PrintParamTest, IntInput){
  funcwatch_param p;
  funcwatch_param_initialize(&p);
  p.name = "param";
  p.func_name = "printParamTest";
  p.call_num = 0;
  p.type = "int";
  p.size = sizeof(int);
  p.addr = (long long unsigned int)(void *)&p;
  p.value = 1;
  p.value_float = -1;

  DynString stringToPrint = print_param(&p, 0);
  
  dynstring_inner_free(stringToPrint);
}
