#include "../funcwatch.h"
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
  p.addr = (Dwarf_Addr)(void *)&p;
  p.value = 1;
  p.value_float = -1;
  p.flags |= FW_INT;

  DynString stringToPrint = print_param(&p, 0);
  char *param_print = stringToPrint.text;
  char *expected_print = "0, printParamTest, 0, param, 4, 00000000000000000000000000000001, int, 1\n";
  ASSERT_STREQ(expected_print, param_print);
  dynstring_inner_free(stringToPrint);
}

TEST(PrintParamTest, UnsupportTypeInput){
  funcwatch_param p;
  funcwatch_param_initialize(&p);
  p.name = "param";
  p.func_name = "printParamTest";
  p.call_num = 0;
  p.type = "unsupported";
  p.size = 0;
  p.addr = (Dwarf_Addr)(void *)&p;
  p.value = 1;
  p.value_float = -1;

  DynString stringToPrint = print_param(&p, 0);
  char *param_print = stringToPrint.text;
  char *expected_print = "0, printParamTest, 0, param, 0, 00000000000000000000000000000000, unsupported, unsupported value\n";
  ASSERT_STREQ(expected_print, param_print);
  dynstring_inner_free(stringToPrint);
}

TEST(PrintParamTest, InvalidInput){
  funcwatch_param p;
  funcwatch_param_initialize(&p);
  p.name = "param";
  p.func_name = "printParamTest";
  p.call_num = 0;
  p.type = "int";
  p.size = 0;
  p.addr = (Dwarf_Addr)(void *)&p;
  p.value = 1;
  p.value_float = -1;
  p.flags |= FW_INVALID;

  DynString stringToPrint = print_param(&p, 0);
  char *param_print = stringToPrint.text;
  char *expected_print = "0, printParamTest, 0, param, 0, 10000000000000000000000000000000, int, unsupported value\n";
  ASSERT_STREQ(expected_print, param_print);
  dynstring_inner_free(stringToPrint);
}

TEST(PrintParamTest, NullPointerInput){
  funcwatch_param p;
  funcwatch_param_initialize(&p);
  p.name = "param";
  p.func_name = "printParamTest";
  p.call_num = 0;
  p.type = "*";
  p.size = sizeof(void *);
  p.addr = (Dwarf_Addr)(void *)&p;
  p.value = 0;
  p.value_float = -1;
  p.flags |= FW_POINTER;

  DynString stringToPrint = print_param(&p, 0);
  char *param_print = stringToPrint.text;
  char *expected_print = "0, printParamTest, 0, param, 4, 00000000000000000000000000001000, *, null\n";
  ASSERT_STREQ(expected_print, param_print);
  dynstring_inner_free(stringToPrint);
}
