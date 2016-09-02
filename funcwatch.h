#ifndef _funcwatch_H
#define _funcwatch_H
#include <stdio.h>
#include <sys/types.h>
#include <stdint.h>
#include <libdwarf.h>
#include <dwarf.h>
#include <sys/types.h>
#include <sys/user.h>
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
  char *type; // human readable
  // these two members are used when funcwatch collects parameters' values
  // however, dwarf_die may be freed after dwarf_finish,
  // they should not be used after funcwatch_run_program finishes
  Dwarf_Die type_die, var_die;
  size_t size; // bytes
  // the address of the var_die.
  // if the parameter is a struct,
  // the address helps to calculate its members
  Dwarf_Addr addr;
  // the level of this param in a struct OR union param
  int struct_level;
  unsigned long value;
  double value_float;
  int flags;
  struct funcwatch_param *next;
};

typedef struct funcwatch_param funcwatch_param;

typedef struct {
  char *prog_name, *func_name;
  funcwatch_param **params;
  funcwatch_param **ret_params;
  funcwatch_param *return_values;
  int num_calls;
  int num_params;
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
void funcwatch_get_params(funcwatch_run *run, int is_return);

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
#define FW_INVALID 0x10000000
//flags for internal use
#define FW_REG_VALUE 0x01
#endif /* _funcwatch_H */
