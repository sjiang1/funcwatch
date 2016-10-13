#include "../funcwatch.h"
#include "../funcwatch_output.h"
#include "../funcwatch_param_util.h"
#include "../dynstring.h"
#include "../stringutil.h"
#include "../vector.h"
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

TEST(PrintParamTest, SignedIntInput){
  funcwatch_param p;
  funcwatch_param_initialize(&p);
  p.name = "param";
  p.func_name = "printParamTest";
  p.call_num = 0;
  p.type = "int";
  p.size = sizeof(int);
  p.addr = (Dwarf_Addr)(void *)&p;
  p.value = -1;
  p.value_float = -1;
  p.flags |= FW_INT;
  p.flags |= FW_SIGNED;

  DynString stringToPrint = print_param(&p, 0);
  char *param_print = stringToPrint.text;
  char *expected_print = "0, printParamTest, 0, param, 4, 00000000000000000000000100000001, int, -1\n";
  ASSERT_STREQ(expected_print, param_print);
  dynstring_inner_free(stringToPrint);
}

TEST(PrintParamTest, UnsignedIntInput){
  funcwatch_param p;
  funcwatch_param_initialize(&p);
  p.name = "param";
  p.func_name = "printParamTest";
  p.call_num = 0;
  p.type = "int";
  p.size = sizeof(int);
  p.addr = (Dwarf_Addr)(void *)&p;
  p.value = -1;
  p.value_float = -1;
  p.flags |= FW_INT;

  DynString stringToPrint = print_param(&p, 0);
  char *param_print = stringToPrint.text;
  char *expected_print = "0, printParamTest, 0, param, 4, 00000000000000000000000000000001, int, 4294967295\n";
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

TEST(PrintParamTest, NonNullPointerInput){
  funcwatch_param p;
  funcwatch_param_initialize(&p);
  p.name = "param";
  p.func_name = "printParamTest";
  p.call_num = 0;
  p.type = "*";
  p.size = sizeof(void *);
  p.addr = (Dwarf_Addr)(void *)&p;
  p.value = (Dwarf_Addr)(void *)&p;
  p.value_float = -1;
  p.flags |= FW_POINTER;

  DynString stringToPrint = print_param(&p, 0);
  char *param_print = stringToPrint.text;
  char *expected_print = "0, printParamTest, 0, param, 4, 00000000000000000000000000001000, *, [memory addr]\n";
  ASSERT_STREQ(expected_print, param_print);
  dynstring_inner_free(stringToPrint);
}

TEST(PrintParamTest, SignedCharInput){
  funcwatch_param p;
  funcwatch_param_initialize(&p);
  p.name = "param";
  p.func_name = "printParamTest";
  p.call_num = 0;
  p.type = "char";
  p.size = sizeof(char);
  p.addr = (Dwarf_Addr)(void *)&p;
  p.value = -1;
  p.value_float = -1;
  p.flags |= FW_CHAR;
  p.flags |= FW_SIGNED;

  DynString stringToPrint = print_param(&p, 0);
  char *param_print = stringToPrint.text;
  char *expected_print = "0, printParamTest, 0, param, 1, 00000000000000000000000100000010, char, -1\n";
  ASSERT_STREQ(expected_print, param_print);
  dynstring_inner_free(stringToPrint);
}

TEST(PrintParamTest, UnsignedCharInput){
  funcwatch_param p;
  funcwatch_param_initialize(&p);
  p.name = "param";
  p.func_name = "printParamTest";
  p.call_num = 0;
  p.type = "unsigned char";
  p.size = sizeof(unsigned char);
  p.addr = (Dwarf_Addr)(void *)&p;
  unsigned char value = -1;
  memcpy(&p.value, &value, 1);
  p.value_float = -1;
  p.flags |= FW_CHAR;

  DynString stringToPrint = print_param(&p, 0);
  char *param_print = stringToPrint.text;
  char *expected_print = "0, printParamTest, 0, param, 1, 00000000000000000000000000000010, unsigned char, 255\n";
  ASSERT_STREQ(expected_print, param_print);
  dynstring_inner_free(stringToPrint);
}

TEST(PrintParamTest, FloatInput){
  funcwatch_param p;
  funcwatch_param_initialize(&p);
  p.name = "param";
  p.func_name = "printParamTest";
  p.call_num = 0;
  p.type = "float";
  p.size = sizeof(float);
  p.addr = (Dwarf_Addr)(void *)&p;
  float value = 123.12345678;
  memcpy(&p.value_float, &value, sizeof(float));
  p.flags |= FW_FLOAT;

  DynString stringToPrint = print_param(&p, 0);
  char *param_print = stringToPrint.text;
  char *expected_print = "0, printParamTest, 0, param, 4, 00000000000000000000000000000100, float, 123.123459\n";
  ASSERT_STREQ(expected_print, param_print);
  dynstring_inner_free(stringToPrint);
}

enum DAY            /* Defines an enumeration type    */
  {
    saturday,       /* Names day and declares a       */
    sunday = 0,     /* variable named workday with    */
    monday,         /* that type                      */
    tuesday,
    wednesday,      /* wednesday is associated with 3 */
    thursday,
    friday
  };

TEST(PrintParamTest, EnumInputTest){
  funcwatch_param p;
  funcwatch_param_initialize(&p);
  p.name = "param";
  p.func_name = "printParamTest";
  p.call_num = 0;
  p.type = "enum DAY";
  p.size = sizeof(int);
  p.addr = (Dwarf_Addr)(void *)&p;
  const char *value = "monday";
  memcpy(&p.value, &value, sizeof(char *));
  p.flags |= FW_ENUM;

  DynString stringToPrint = print_param(&p, 0);
  char *param_print = stringToPrint.text;
  char *expected_print = "0, printParamTest, 0, param, 4, 00000000000000000000000001000000, enum DAY, monday\n";
  ASSERT_STREQ(expected_print, param_print);
  dynstring_inner_free(stringToPrint);
}

struct Books {
  char  title[50];
  char  author[50];
  char  subject[100];
  int   book_id;
};

TEST(PrintParamTest, StructInputTest){
  funcwatch_param p;
  funcwatch_param_initialize(&p);
  p.name = "param";
  p.func_name = "printParamTest";
  p.call_num = 0;
  p.type = "struct Books";
  p.size = sizeof(struct Books);
  p.addr = (Dwarf_Addr)(void *)&p;
  p.flags |= FW_STRUCT;

  DynString stringToPrint = print_param(&p, 0);
  char *param_print = stringToPrint.text;
  char *expected_print = "0, printParamTest, 0, param, 204, 00000000000000000000000000010000, struct Books, \n";
  ASSERT_STREQ(expected_print, param_print);
  dynstring_inner_free(stringToPrint);
}

TEST(PrintParamTest, DefaultInputTest){
  funcwatch_param p;
  funcwatch_param_initialize(&p);
  p.name = "param";
  p.func_name = "printParamTest";
  p.call_num = 0;
  p.type = "unknown";
  p.addr = (Dwarf_Addr)(void *)&p;

  DynString stringToPrint = print_param(&p, 0);
  char *param_print = stringToPrint.text;
  char *expected_print = "0, printParamTest, 0, param, 0, 00000000000000000000000000000000, unknown, no flag to identify the type\n";
  ASSERT_STREQ(expected_print, param_print);
  dynstring_inner_free(stringToPrint);
}

TEST(PrintParamListTest, EmptyListTest){
  funcwatch_param *head = 0;
  DynString stringToPrint = print_param_list(head, 1);

  int stringLength = strlen(stringToPrint.text);
  ASSERT_EQ(0, stringLength);
  dynstring_inner_free(stringToPrint);
}

TEST(PrintParamListTest, OneElementListTest){
  funcwatch_param p;
  funcwatch_param_initialize(&p);

  DynString stringToPrint = print_param_list(&p, 1);
  char *expected_print = "1, (null), -1, (null), 0, 00000000000000000000000000000000, (null), no flag to identify the type\n";
  
  ASSERT_STREQ(expected_print, stringToPrint.text);
  dynstring_inner_free(stringToPrint);
}

TEST(PrintParamListTest, TwoElementsListTest){
  funcwatch_param p1, p2;
  funcwatch_param_initialize(&p1);
  funcwatch_param_initialize(&p2);
  
  p1.name = "param1";
  p1.func_name = "printParamTest";
  p1.call_num = 0;
  p1.type = "*";
  p1.size = sizeof(void *);
  p1.addr = (Dwarf_Addr)(void *)&p1;
  p1.value = (Dwarf_Addr)(void *)&p1;
  p1.value_float = -1;
  p1.flags |= FW_POINTER;

  p2.name = "param2";
  p2.func_name = "printParamTest";
  p2.call_num = 0;
  p2.type = "*";
  p2.size = sizeof(void *);
  p2.addr = (Dwarf_Addr)(void *)&p2;
  p2.value = 0;
  p2.value_float = -1;
  p2.flags |= FW_POINTER;

  p1.next = &p2;
  
  DynString stringToPrint = print_param_list(&p1,1);
  char *expected_print = "1, printParamTest, 0, param1, 4, 00000000000000000000000000001000, *, [memory addr]\n1, printParamTest, 0, param2, 4, 00000000000000000000000000001000, *, null\n";
  
  ASSERT_STREQ(expected_print, stringToPrint.text);
  dynstring_inner_free(stringToPrint);
}

TEST(PrintParamVectorTest, EmptyVector){
  Vector v;
  vector_init(&v);
  DynString stringToPrint = print_param_vector(v, 0);
  char *expected_print = "";
  ASSERT_STREQ(expected_print, stringToPrint.text);
  dynstring_inner_free(stringToPrint);
  free(v.data);
}

TEST(PrintParamVectorTest, OneItemVector){
  Vector v;
  vector_init(&v);

  funcwatch_param p;
  funcwatch_param_initialize(&p);
  vector_append(&v, &p);
  
  DynString stringToPrint = print_param_vector(v,0);
  char *expected_print = "0, (null), -1, (null), 0, 00000000000000000000000000000000, (null), no flag to identify the type\n";
  ASSERT_STREQ(expected_print, stringToPrint.text);
  dynstring_inner_free(stringToPrint);
  free(v.data);
}

TEST(PrintParamVectorTest, OneListVector){
  Vector v;
  vector_init(&v);

  funcwatch_param p1, p2;
  funcwatch_param_initialize(&p1);
  funcwatch_param_initialize(&p2);
  
  p1.name = "param1";
  p1.func_name = "printParamTest";
  p1.call_num = 0;
  p1.type = "*";
  p1.size = sizeof(void *);
  p1.addr = (Dwarf_Addr)(void *)&p1;
  p1.value = (Dwarf_Addr)(void *)&p1;
  p1.value_float = -1;
  p1.flags |= FW_POINTER;

  p2.name = "param2";
  p2.func_name = "printParamTest";
  p2.call_num = 0;
  p2.type = "*";
  p2.size = sizeof(void *);
  p2.addr = (Dwarf_Addr)(void *)&p2;
  p2.value = 0;
  p2.value_float = -1;
  p2.flags |= FW_POINTER;

  p1.next = &p2;

  vector_append(&v, &p1);
  
  DynString stringToPrint = print_param_vector(v,0);
  char *expected_print = "0, printParamTest, 0, param1, 4, 00000000000000000000000000001000, *, [memory addr]\n0, printParamTest, 0, param2, 4, 00000000000000000000000000001000, *, null\n";
  ASSERT_STREQ(expected_print, stringToPrint.text);
  dynstring_inner_free(stringToPrint);
  free(v.data);
}

TEST(PrintParamVectorTest, TwoItemsVector){
  Vector v;
  vector_init(&v);

  funcwatch_param p;
  funcwatch_param_initialize(&p);
  vector_append(&v, &p);

  funcwatch_param p1, p2;
  funcwatch_param_initialize(&p1);
  funcwatch_param_initialize(&p2);
  
  p1.name = "param1";
  p1.func_name = "printParamTest";
  p1.call_num = 0;
  p1.type = "*";
  p1.size = sizeof(void *);
  p1.addr = (Dwarf_Addr)(void *)&p1;
  p1.value = (Dwarf_Addr)(void *)&p1;
  p1.value_float = -1;
  p1.flags |= FW_POINTER;

  p2.name = "param2";
  p2.func_name = "printParamTest";
  p2.call_num = 0;
  p2.type = "*";
  p2.size = sizeof(void *);
  p2.addr = (Dwarf_Addr)(void *)&p2;
  p2.value = 0;
  p2.value_float = -1;
  p2.flags |= FW_POINTER;

  p1.next = &p2;
  vector_append(&v, &p1);
  
  DynString stringToPrint = print_param_vector(v,0);
  char *expected_print = "0, (null), -1, (null), 0, 00000000000000000000000000000000, (null), no flag to identify the type\n0, printParamTest, 0, param1, 4, 00000000000000000000000000001000, *, [memory addr]\n0, printParamTest, 0, param2, 4, 00000000000000000000000000001000, *, null\n";
  ASSERT_STREQ(expected_print, stringToPrint.text);
  dynstring_inner_free(stringToPrint);
  free(v.data);  
}


/*
 *
 * Start testing output_logged_values
 *
 */

// utility funciton
void initialize_funcwatch_run(funcwatch_run * run){
  run->prog_name = "XProgram";
  run->func_name = "XFunction";
  run->num_calls = 0;
  run->num_rets = 0;
}

TEST(OutputLoggedValuesTest, EmptyRun){
  funcwatch_run run;
  initialize_funcwatch_run(&run);
  DynString stringToPrint = output_logged_values_inner(&run);
  char *expected_print = "";
  ASSERT_STREQ(expected_print, stringToPrint.text);
}

TEST(OutputLoggedValuesTest, OneEmptyCallRun){
  funcwatch_run run;
  initialize_funcwatch_run(&run);
  Vector params, ret_params;
  vector_init(&params);
  vector_init(&ret_params);
  
  run.params[0] = params;
  run.ret_params[0] = ret_params;
  run.num_calls = 1;
  run.num_rets = 0;
  
  DynString stringToPrint = output_logged_values_inner(&run);
  char *expected_print = "";
  ASSERT_STREQ(expected_print, stringToPrint.text);
  free(params.data);
  free(ret_params.data);
}

TEST(OutputLoggedValuesTest, OneTwoParamCallRun){
  funcwatch_run run;
  initialize_funcwatch_run(&run);
  Vector params, ret_params;
  vector_init(&params);
  vector_init(&ret_params);
  
  funcwatch_param p;
  funcwatch_param_initialize(&p);
  p.call_num = 0;
  p.name = "param1";
  vector_append(&params, &p);

  funcwatch_param p1, p2;
  funcwatch_param_initialize(&p1);
  funcwatch_param_initialize(&p2);
  
  p1.name = "param2";
  p1.func_name = "printParamTest";
  p1.call_num = 0;
  p1.type = "*";
  p1.size = sizeof(void *);
  p1.addr = (Dwarf_Addr)(void *)&p1;
  p1.value = (Dwarf_Addr)(void *)&p1;
  p1.value_float = -1;
  p1.flags |= FW_POINTER;

  p2.name = "param2.1";
  p2.func_name = "printParamTest";
  p2.call_num = 0;
  p2.type = "*";
  p2.size = sizeof(void *);
  p2.addr = (Dwarf_Addr)(void *)&p2;
  p2.value = 0;
  p2.value_float = -1;
  p2.flags |= FW_POINTER;

  p1.next = &p2;
  vector_append(&params, &p1);

  // Update ret_params
  funcwatch_param ret_p;
  funcwatch_param_initialize(&ret_p);
  ret_p.call_num = 0;
  ret_p.name = "param1";
  vector_append(&ret_params, &ret_p);

  funcwatch_param ret_p1, ret_p2;
  funcwatch_param_initialize(&ret_p1);
  funcwatch_param_initialize(&ret_p2);
  
  ret_p1.name = "param2";
  ret_p1.func_name = "printParamTest";
  ret_p1.call_num = 0;
  ret_p1.type = "*";
  ret_p1.size = sizeof(void *);
  ret_p1.addr = (Dwarf_Addr)(void *)&ret_p1;
  ret_p1.value = (Dwarf_Addr)(void *)&ret_p1;
  ret_p1.value_float = -1;
  ret_p1.flags |= FW_POINTER;

  ret_p2.name = "param2.1";
  ret_p2.func_name = "printParamTest";
  ret_p2.call_num = 0;
  ret_p2.type = "*";
  ret_p2.size = sizeof(void *);
  ret_p2.addr = (Dwarf_Addr)(void *)&ret_p2;
  ret_p2.value = -1;
  ret_p2.value_float = -1;
  ret_p2.flags |= FW_POINTER;

  ret_p1.next = &ret_p2;
  vector_append(&ret_params, &ret_p1);

  run.params[0] = params;
  run.ret_params[0] = ret_params;
  run.num_calls = 1;
  run.num_rets = 0;
  
  DynString stringToPrint = output_logged_values_inner(&run);
  char *expected_print = "0, (null), 0, param1, 0, 00000000000000000000000000000000, (null), no flag to identify the type\n0, printParamTest, 0, param2, 4, 00000000000000000000000000001000, *, [memory addr]\n0, printParamTest, 0, param2.1, 4, 00000000000000000000000000001000, *, null\n1, (null), 0, param1, 0, 00000000000000000000000000000000, (null), no flag to identify the type\n1, printParamTest, 0, param2, 4, 00000000000000000000000000001000, *, [memory addr]\n1, printParamTest, 0, param2.1, 4, 00000000000000000000000000001000, *, [memory addr]\n";
  ASSERT_STREQ(expected_print, stringToPrint.text);
  free(params.data);
  free(ret_params.data);
}
