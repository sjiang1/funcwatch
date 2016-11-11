#ifndef _funcwatch_H
#define _funcwatch_H
#include <stdio.h>
#include <sys/types.h>
#include <stdint.h>
#include <libdwarf.h>
#include <dwarf.h>
#include <sys/types.h>
#include <sys/user.h>
#include <stdint.h>
#include "vector.h"

#define DEBUG 1
#define MAX_CALL_CNT 100
#define PRINT_MEM_ADDR 1

typedef struct user_regs_struct user_regs_struct;
typedef struct user_fpregs_struct user_fpregs_struct;

#define MAX_RESOLVE_DEPTH 10

typedef struct {
  Dwarf_Unsigned *base;
  Dwarf_Unsigned length; // length used by the evaluator
  Dwarf_Unsigned max; // maximum number of entries before we have to reallocate
} expression_stack; // for dwarf expressions

struct funcwatch_param{
  char *name;
  char *func_name;
  int call_num;
  
  // these two members are used when funcwatch collects parameters' values
  // however, dwarf_die may be freed after dwarf_finish,
  // they should not be used after funcwatch_run_program finishes
  Dwarf_Die type_die, var_die;

  /*--type-related--*/
  // human readable
  char *type;
  // size in bytes
  size_t size;
  // type flags, including INVALID,
  // which indicates the param is invalid.
  int flags;
  /*----------------*/

  /*--value-related--*/
  Dwarf_Addr addr;
  uint64_t value;
  long double value_float;
  /*-----------------*/
  
  // the level of this param in a struct OR union param
  int struct_level;

  struct funcwatch_param *next;
};

typedef struct funcwatch_param funcwatch_param;

typedef struct {
  char *prog_name, *func_name;
  /* Vector Array: params
   * Each vector stores parameters values for one function call
   * e.g. 
   *   params[0] : Vector 0: call 0
   *   params[1] : Vector 1: call 1
   *   etc.

   * A vector is a dynamic array of lists of params
   * i.e. vector = (list<param>)[], an array of lists
   *   each element in the vector stores the head of the list
   *   each list denotes a parameter
   * e.g.
   *   vector[0] : param 0 -> param 0.field1 -> param0.field2
   *   vector[1] : param 1 
   *   etc.

   * Each list contains all the values that compose a parameter
   */
  Vector params[MAX_CALL_CNT]; 
  Vector ret_params[MAX_CALL_CNT];
  funcwatch_param return_values[MAX_CALL_CNT];
  int num_calls; // every time the num_calls increases, there will be new params added to params
  int num_rets;  // the number of calls that have been returned. every time the num_rets increases, there should be new returns added to ret_params and return_values
  
  /* 
   * The following fields should be discarded 
   * once funcwatch_run_program finishes 
   */
  Vector call_stack; // the call stack for the function calls. used for recursive ones to identify each different call
  int fd; // file descriptor for the scene. Passed to libdwarf and any other libraries that need to read it.
  int mem_fd; //file descriptor for reading and writing process memory
  Dwarf_Debug dwarf_ptr;
  Dwarf_Error dwarf_err;
  pid_t child_pid;
  // the dwarf function die object may be freed after dwarf_finish
  // this member should not be used after funcwatch_run_program finishes
  Dwarf_Die function_die; // the dwarf function object.
} funcwatch_run;


//function prototypes for solving dwarf expressions
expression_stack *create_stack();
void parse_location(funcwatch_run *run, Dwarf_Loc* loc, expression_stack *stack, expression_stack *flags_stack, user_regs_struct *registers, Dwarf_Unsigned fbreg);
Dwarf_Unsigned call_frame_cfa(funcwatch_run *run, user_regs_struct *registers);
void free_stack(expression_stack *stack);
Dwarf_Unsigned pop(expression_stack *stack);
void push(expression_stack *stack, Dwarf_Unsigned p);
int evaluate_address(funcwatch_run *run, Dwarf_Die *die, Dwarf_Half tag, Dwarf_Unsigned fbreg, int *flags, Dwarf_Unsigned *ret_address);

funcwatch_run *funcwatch_run_program(char *prog_name,  char *func_name, char **argv);

#define FW_INT 0x01
#define FW_CHAR 0x02
#define FW_FLOAT 0x04
#define FW_POINTER 0x08
#define FW_STRUCT 0x10
#define FW_TYPEDEF 0x20
#define FW_ENUM 0x40
#define FW_VOID 0x80
#define FW_SIGNED 0x100
#define FW_UNION 0x200
#define FW_CONSTANT 0x400
#define FW_ARRAY 0x800
#define FW_INVALID 0x1000
//flags for internal use
#define FW_REG_VALUE 0x01
#endif /* _funcwatch_H */
