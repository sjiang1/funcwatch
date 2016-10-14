#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <inttypes.h>
#include <unistd.h>
#include <getopt.h>
#include <dlfcn.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <sys/ptrace.h>
#include <sys/uio.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <sys/uio.h>
#include <sys/prctl.h>

#include <dwarf.h>
#include <libdwarf.h>
#include <udis86.h>
#include <debug_printf.h>
#include "arch.h"

#include "funcwatch.h"
#include "vector.h"
#include "stringutil.h"
#include "dwarf_util.h"
#include "funcwatch_param_util.h"

void funcwatch_get_params(funcwatch_run *run);

static void get_value(funcwatch_param *p, funcwatch_run *run, Dwarf_Unsigned fbreg);
static void get_value_from_remote_process_inner(funcwatch_param *param, pid_t pid);

static funcwatch_param *resolve_string(funcwatch_run *run, funcwatch_param *p);
static funcwatch_param *resolve_pointer(funcwatch_run *run, funcwatch_param *p);
static funcwatch_param *resolve_enum(funcwatch_run *run, funcwatch_param *p);
static funcwatch_param *resolve_struct(funcwatch_run *run, funcwatch_param *p, int is_resolve_pointer);
void copy_changes(pid_t pid, unsigned long *old_buf, unsigned long *new_buf, unsigned  long address, size_t size);
//begin content


static void get_value(funcwatch_param *p, funcwatch_run *run, Dwarf_Unsigned fbreg){
  Dwarf_Unsigned address = 0;
  int flags = 0;
  int eval_res = evaluate_address(run, &(p->var_die), DW_AT_location, fbreg, &flags, &address);
  if(flags & FW_INVALID){
    p->flags |= FW_INVALID;
  }
  else if(flags & FW_REG_VALUE){
    // evalute_address will extract the value at the register
    p->value = address;
    p->addr = 0;
    debug_printf("Warning: There is a bug regarding to evalute_address from a register. \
                  Parameter %s is skipped.\n", p->name);
  }
  else {
    void *tmpptr = (void *)address;
    p->addr = tmpptr;
    debug_printf("Located parameter %s at %p\n", p->name, tmpptr);
    if(p->flags & FW_STRUCT && !(p->flags & FW_POINTER))
      p->value = 0;
    else if(p->flags & FW_UNION && !(p->flags & FW_POINTER))
      p->value = 0;
    else
      get_value_from_remote_process_inner(p, run->child_pid);
  }
}

/*
 * Get the value from process pid to assign the value to param
 * for integer types,
 * we get the raw data to the uint64_t type

 * for pointer type, we pass the data as it is
 * for char type, we assigne char value into &param->value
 * for enum type, we pass the data as it is

 * for floating types,
 *     for float type, we pass the data as it is
 *     for double, long double types, we type cast them to float types, and pass the data as float
 *
 * for struct types ******************
 *     this function does not accept struct types! we do not obtain struct types from remote process
 */
void get_value_from_remote_process_inner(funcwatch_param *param, pid_t pid){
  errno = 0;
	  
  if(param->flags & FW_STRUCT && !(param->flags & FW_POINTER)){
    debug_printf("Error: %s\n", "funcwatch does not access struct's values from remote process directly.");
    return;
  }

  if(param->flags & FW_UNION && !(param->flags & FW_POINTER)){
    debug_printf("Error: %s\n", "funcwatch does not access union's values from remote process directly.");
    return;
  }

  void *remote_address = (void *) param->addr;
  param->value = 0;
  if(param->flags & FW_POINTER || param->flags & FW_ENUM) {
    param->value = ptrace(PTRACE_PEEKDATA, pid,  remote_address, 0);
  }
  else if(param->flags & FW_INT) {
    switch(param->size) {
    case 2 :
    case 4 : {
      long tmpdata = ptrace(PTRACE_PEEKDATA, pid,  remote_address, 0);
      long *valueptr = (long *)&(param->value);
      memcpy(valueptr, &tmpdata, sizeof(long));
      break;
    }
    case 8 : {
      long first = 0;
      long second = 0;
      first = ptrace(PTRACE_PEEKDATA, pid, remote_address, 0);
      second = ptrace(PTRACE_PEEKDATA, pid, remote_address+sizeof(long), 0);
      
      long *tmpptr = (long *)&(param->value);
      memcpy(tmpptr, &first, sizeof(first));
      memcpy(tmpptr+1, &second, sizeof(second));      
      break;
    }
    default:
      debug_printf("Warning: unsupported int size: %zu", param->size);
      debug_printf(" Parameter name: %s", param->name);
      param->type = "unsupported";
      break;
    }
  } // end ints
  else if(param->flags & FW_CHAR){
    long tmpdata = ptrace(PTRACE_PEEKDATA, pid, remote_address, 0);
    param->value = tmpdata;
  }
  else if(param->flags & FW_FLOAT) {
    if(param->size == sizeof(float))
      param->value = ptrace(PTRACE_PEEKDATA, pid, (float *)remote_address, 0);
    else if(param->size == sizeof(double)){
      long first = ptrace(PTRACE_PEEKDATA, pid, remote_address, 0);
      long second = ptrace(PTRACE_PEEKDATA, pid, remote_address+sizeof(long), 0);
      double d_data = 0;
      long *tmpptr = (long *)&d_data;
      memcpy(tmpptr, &first, sizeof(first));
      memcpy(tmpptr+1, &second, sizeof(second));
      param->value_float = 0;
      double *value_ptr = &(param->value_float);
      memcpy(value_ptr, &d_data, sizeof(double));
    }
    else if(param->size == sizeof(long double)){
      long first = ptrace(PTRACE_PEEKDATA, pid, remote_address, 0);
      long second = ptrace(PTRACE_PEEKDATA, pid, remote_address+sizeof(long), 0);
      long third = ptrace(PTRACE_PEEKDATA, pid, remote_address+2*sizeof(long), 0);
      long double ld_data = 0;
      long *tmpptr = (long *)&ld_data;
      memcpy(tmpptr, &first, sizeof(first));
      memcpy(tmpptr+1, &second, sizeof(second));
      memcpy(tmpptr+2, &third, sizeof(long double) - sizeof(first) - sizeof(second));
      param->value_float = ld_data;
    }
    else {
      debug_printf("Warning: unsupported float size: %zu", param->size);
      debug_printf(" Parameter name: %s", param->name);
      param->type = "unsupported";
    }
  } //end floats
  else {
    debug_printf("Unsupported type. Parameter name: %s ", param->name);
    debug_printf("type name: %s\n", param->type);
    param->type = "unsupported";
  }

  if(errno != 0) {
    debug_printf("Unable to get variable value: %s\n", strerror(errno));
    param->value = 0;
    param->value_float = 0;
    param->flags |= FW_INVALID;
  }
  else
    return;
}

/*
 * Get params for a call
 */
void funcwatch_get_params(funcwatch_run *run) {
  int n_params = 0;
  Vector params_to_get;
  vector_init(&params_to_get);
  
  long rc = 0;
  errno = 0;
  int flags = 0;
  Dwarf_Unsigned fbreg= 0;
  int eval_res = evaluate_address(run, &(run->function_die), DW_AT_frame_base, 0, &flags, &fbreg);
  if (eval_res != DW_DLV_OK){
    debug_printf("Did not get the address of the function successfully. Exit.%s", "\n");
    exit(-1);
  }
  
  Dwarf_Die var_die;
  Dwarf_Error err = err;
  Dwarf_Half tag;
  dwarf_child(run->function_die, &var_die, &err);

  while(1) {
    if (dwarf_tag(var_die, &tag, &err) != DW_DLV_OK)
      return;
    char *var_name;         

    if (tag == DW_TAG_formal_parameter) {
      rc = dwarf_diename(var_die, &var_name, &err);
      if(rc < 0){
	var_name = NULL;
      }
      
      ++ n_params;
      funcwatch_param *p = (funcwatch_param *) malloc(sizeof(funcwatch_param));
      vector_append(&params_to_get, p);
      
      funcwatch_param_initialize(p);
      p->name = var_name;
      p->func_name = run->func_name;
      p->call_num = run->num_calls-1;
      p->var_die = var_die;
      get_type_info_from_var_die(run->dwarf_ptr, var_die, p);

      if(p->size > 0 && p->size < 4 && !(p->flags & FW_POINTER)){
	debug_printf("Warning: parameter %s, \
                      there is a bug regarding to char/short as a parameter. \
                      the collected value may be incorrect.\n",
		     p->name);
      }
      
      //now find out where it is, and get the value of the parameter
      get_value(p, run, fbreg);

      if(p->flags & FW_INVALID){
	p->value = 0;
      }
      else if(p->flags & FW_POINTER && p->flags & FW_CHAR){
	resolve_string(run, p);
      }
      else if(p->flags & FW_POINTER){
	resolve_pointer(run, p);
      }
      else if(p->flags & FW_ENUM){
	resolve_enum(run, p);
      }
      else if(p->flags & FW_UNION){
	resolve_struct(run, p, 0);
      }
      else if(p->flags &FW_STRUCT){
	resolve_struct(run, p, 1);
      }
      
    }
    
    // the var_die is not freed, because right now, they are stored in funcwatch_param.var_die
    int rc = dwarf_siblingof(run->dwarf_ptr, var_die, &var_die, &err);
    
    if (rc == DW_DLV_ERROR) {
      debug_printf("%s", dwarf_errmsg(err));
      return;
    }
    else if (rc == DW_DLV_NO_ENTRY)
      break; /* done */

  }

  if(n_params > 0) {
    run->params[run->num_calls-1] = params_to_get;
  }else{
    free(params_to_get.data);
  }
}

void reevaluate_params(funcwatch_run *run){
  // Get the call id
  int call_id = *(int *)vector_last(&(run->call_stack));
  int n_params = 0;
  Vector params_to_get;
  vector_init(&params_to_get);
  
  // get the parameters by the call id
  Vector * params_got = get_param_of_call_id(run->params, run->num_calls, call_id); 

  for(int param_index = 0; param_index < params_got->size; param_index ++){
    funcwatch_param * param_got = vector_get(params_got, param_index);
    ++ n_params;

    funcwatch_param *p = (funcwatch_param *) malloc(sizeof(funcwatch_param));
    funcwatch_param_initialize(p);
    vector_append(&params_to_get, p);
    
    p->name = param_got->name;
    p->func_name = run->func_name;
    p->call_num = call_id;
    
    p->var_die = param_got->var_die;
    p->type_die = param_got->type_die;
    p->flags = param_got->flags;
    p->type = param_got->type;
    p->size = param_got->size;

    p->value = param_got->value;
    p->addr = param_got->addr;
    
    if( !(p->flags & FW_INVALID) ){
      if(p->flags & FW_POINTER && p->flags & FW_CHAR)
	p = resolve_string(run, p);
      else if(p->flags & FW_POINTER)
	p = resolve_pointer(run, p);
      else if(p->flags & FW_ENUM)
	p = resolve_enum(run, p);
      else if(p->flags & FW_UNION)
	p = resolve_struct(run, p, 0);
      else if(p->flags &FW_STRUCT)
	p = resolve_struct(run, p, 1);
    }
    
    // go to next parameter
    param_got = param_got->next;
  }  
  
  // 
  if(n_params > 0) {
    run->ret_params[run->num_rets-1] = params_to_get;
  }else{
    free(params_to_get.data);
  }
}

funcwatch_run *funcwatch_run_program(char *prog_name,  char *func_name, char **argv)
{
  funcwatch_run *run = malloc(sizeof(funcwatch_run));
  run->num_calls = 0;
  run->num_rets = 0;
  vector_init(&(run->call_stack));
  run->prog_name = prog_name;
  run->func_name = func_name;
  
  Dwarf_Error err;
  run->fd = open(run->prog_name, O_RDONLY);
  if(run->fd < 0) {
    printf("Error opening file %s: %s\n", run->prog_name, strerror(errno));
    exit(-1);
  }
  if (dwarf_init(run->fd, DW_DLC_READ, 0, 0, &(run->dwarf_ptr), &err) != DW_DLV_OK) {
    fprintf(stderr, "Failed DWARF initialization:%s\n", dwarf_errmsg(err));
    return 0;
  }
  pid_t rc_pid;
  int child_status;
  pid_t child_pid = fork();
  if (child_pid < 1){
    /*
     *  in normal operation the child should exit before the parent.
     *  To insure that the reverse never happens we have the parent send SIGKILL upon exit.
     */
    prctl(PR_SET_PDEATHSIG, SIGKILL);
    int rc = 	ptrace(PTRACE_TRACEME, run->child_pid, 0, 0);
    raise(SIGSTOP);
    //	debug_printf("%s", "After sigstop.\n");
    execv(prog_name, argv);
    debug_printf("Execv error:%s\n", strerror(errno));
    exit(1);
  }
  else{
    // debug_printf("Executing: pid = %d\n", child_pid);
    rc_pid = waitpid(child_pid, &child_status, 0);
    run->child_pid = child_pid;
    int rc = 	ptrace(PTRACE_CONT, run->child_pid, 0, 0);
    rc_pid = waitpid(child_pid, &child_status, 0);
    Dwarf_Error err;
    Dwarf_Unsigned next_cu_header;
    Dwarf_Die no_die = 0;
    Dwarf_Die cu_die = 0;
    Dwarf_Die child_die = 0;
    
    /* Find compilation unit header */
    rc=dwarf_next_cu_header(
			    run->dwarf_ptr,NULL,NULL,NULL,NULL,&next_cu_header, &err);
    if(rc==DW_DLV_ERROR){
      debug_printf("%s\n", dwarf_errmsg(err));
      return 0;
    }
    else if(rc == DW_DLV_NO_ENTRY) {
      debug_printf("%s\n", "No dwarf entries found.");
      return 0;
    }
    //go through all the compilation units, which usually correspond to source files
    while(1) {
      //there should be a sibbling and children, otherwise there's something wrong
      if (cu_die != 0){
	// dwarf_dealloc(run->dwarf_ptr, cu_die, DW_DLA_DIE);
      }
      rc=dwarf_siblingof(run->dwarf_ptr, no_die, &cu_die, &err);
      if(rc==DW_DLV_ERROR){
	return 0;
      }
      // child_die is used to set run->function_die
      rc=dwarf_child(cu_die, &child_die, &err);
      if(rc==DW_DLV_OK) {
	// check each child die for the function we want
	while(1) {
	  char* die_name = 0;
	  const char* tag_name = 0;
	  Dwarf_Error err;
	  Dwarf_Half tag;
	  
	  rc=dwarf_tag(child_die, &tag, &err);
	  if(rc !=DW_DLV_OK) {
	    debug_printf("Error:%s\n", dwarf_errmsg(err));
	    exit(-1);
	  }
	  if (tag == DW_TAG_subprogram) {
	    //found a function, now check if it's our function
	    //also check to make sure it's not a declaration stub
	    Dwarf_Attribute attr;
	    Dwarf_Bool decl;
	    rc = dwarf_attr(child_die,DW_AT_declaration,&attr, &err);
	    if(rc == DW_DLV_NO_ENTRY) {
	      decl = 0;
	    }
	    else if(rc ==DW_DLV_ERROR) {
	      debug_printf("Error:%s\n", dwarf_errmsg(err));
	      exit(-1);
	    }
	    else {
	      rc = dwarf_formflag(attr, &decl, &err);
	      if(rc !=DW_DLV_OK) {
		debug_printf("Error:%s\n", dwarf_errmsg(err));
		exit(-1);
	      }
	    }
	    rc = dwarf_diename(child_die, &die_name, &err);
	    if(rc ==DW_DLV_ERROR) {
	      debug_printf("Error:%s\n", dwarf_errmsg(err));
	      exit(-1);
	    }

	    if(rc == DW_DLV_OK
	       &&strcmp(die_name, run->func_name)==0
	       && decl == 0) {
	      run->function_die=child_die;
	      // Now set a breakpoint at the target function and keep executing until the program exits.
	      //Note that we need to clear and reset the breakpoint each time.
	      Dwarf_Addr address, max_address;
	      rc = dwarf_lowpc(run->function_die, &address, &err);
	      if (rc == DW_DLV_ERROR) {
		debug_printf("Error: %s\n", dwarf_errmsg(err));
		exit(-1);
	      }
	      else if(rc == DW_DLV_NO_ENTRY) {
		// TODO: figure out why we're seeing function entries that aren't declaration stubs but still don't have lowpc
		rc = dwarf_siblingof(run->dwarf_ptr, child_die, &child_die, &err);
		if(rc == DW_DLV_OK)
		  continue;
		if (rc == DW_DLV_ERROR) {
		  debug_printf("Error: %s\n", dwarf_errmsg(err));
		  exit(-1);
		}
		else if (rc == DW_DLV_NO_ENTRY)
		  break;
	      }
	      Dwarf_Half form;
	      enum Dwarf_Form_Class class;
	      rc = dwarf_highpc_b(run->function_die, &max_address, &form, &class, &err);
	      if (rc != DW_DLV_OK) {
		debug_printf("Error: %s\n", dwarf_errmsg(err));
		exit(-1);;
	      }
	      //Dwarf 4 allows highpc to be returned as an offset rather than an absolute address.
	      if(class == DW_FORM_CLASS_CONSTANT)
		max_address += address;
	      size_t sz = max_address-address;
	      while(sz%sizeof(void *) > 0)
		++sz;
	      user_regs_struct registers;
	      user_fpregs_struct fpregisters;
	      rc = ptrace(PTRACE_GETREGS, run->child_pid, 0, &registers);
	      rc = ptrace(PTRACE_GETFPREGS, run->child_pid, 0, &fpregisters);
	      //new_buf is the function with breakpoints at the start and all return points
	      //old_buf is the original
	      uint8_t old_buf[sz];
	      uint8_t new_buf[sz];
	      struct iovec liovec, riovec;
	      liovec.iov_base = old_buf;
	      liovec.iov_len = sz;
	      riovec.iov_base = (void *) address;
	      riovec.iov_len = sz;
	      rc = process_vm_readv(run->child_pid, &liovec, 1, &riovec, 1, 0);
	      if(rc!= sz) {
		debug_printf("read error: %s\n", strerror(errno));
		exit(-1);
	      }
	      memcpy(new_buf, old_buf, sz);
	      //set breakpoints at the entrance and exits of the function
	      set_breakpoints(old_buf, new_buf, max_address-address);
	      copy_changes(run->child_pid, (unsigned long *) old_buf, (unsigned long *) new_buf, address, sz);
	      /*
	      //TODO: switch over to this.
	      liovec.iov_base = new_buf;
	      rc = process_vm_writev(run->child_pid, &liovec, 1, &riovec, 1, 0);
	      */
	      rc = 	ptrace(PTRACE_CONT, run->child_pid, 0, 0);
	      rc_pid = waitpid(child_pid, &child_status, 0);
	      if(WIFEXITED(child_status)) {
		fprintf(stderr, "Warning: the program exited, but function %s was never called. No output will be created.\n", run->func_name);
		debug_printf("Warning: the program exited, but function %s was never called. No output will be created.\n", run->func_name);
		exit(0);
	      }

	      while(1) {
		rc = ptrace(PTRACE_GETREGS, run->child_pid, 0, &registers);
		rc = ptrace(PTRACE_GETFPREGS, run->child_pid, 0, &fpregisters);
		if( rc != 0) {
		  debug_printf("Error getting registers: %s\n", strerror(errno));
		  exit(-1);
		}

		//We need to put the breakpoint back, so we singlestep over this instruction and re-insert it.
		rewind_breakpoint(&registers);
		rc = ptrace(PTRACE_SETREGS, run->child_pid, 0, &registers);
		if( rc != 0) {
		  debug_printf("Error setting registers: %s\n", strerror(errno));
		  exit(-1);
		}
		copy_changes(run->child_pid, (unsigned long *) new_buf, (unsigned long *) old_buf, address, sz);
		//rc = ptrace(PTRACE_GETREGS, run->child_pid, 0, &registers);
		if(get_instruction_pointer(&registers) == address) {
		  ++run->num_calls;
		  if(run->num_calls >= MAX_CALL_CNT){
		    exit(0);
		  }
		  int *call_id = (int *)malloc(sizeof(int));
		  *call_id = run->num_calls - 1;
		  vector_append(&(run->call_stack), call_id);
		  funcwatch_get_params(run);
		}
		else { // get the return value for this run
		  ++run->num_rets;
		  if(run->num_rets >= MAX_CALL_CNT){
		    exit(0);
		  }
		  
		  // this cannot be moved to after getting the return value
		  // in getting the return values,
		  // we removed the last call_id in the run->call_stack
		  reevaluate_params(run);

		  Dwarf_Bool b;
		  dwarf_hasattr(run->function_die, DW_AT_type, &b, &err);
		  if(b) {
		    funcwatch_param *r = run->return_values+run->num_rets-1;
		    r->name = "$return_value";
		    r->func_name = run->func_name;
		    int *call_id = vector_remove_last(&(run->call_stack));
		    r->call_num = *call_id;
		    free(call_id);
		    r->next = 0;
		    r->struct_level = 0;
		    get_type_info_from_var_die(run->dwarf_ptr, run->function_die, r);
		    if(! (r->flags & FW_INVALID)){ 
		      if(r->flags & FW_FLOAT && r->size > sizeof(long int)) {
			r->value = get_return_register_float(&fpregisters);
		      }
		      else{
			r->value = get_return_register(&registers);
			if(r->flags & FW_INT && r->size == 2 && r->flags & FW_SIGNED){
			  short tmp = (short) r->value;
			  r->value = tmp;
			}else if(r->flags & FW_INT && r->size == 2){
			  unsigned short tmp = (unsigned short) r->value;
			  r->value = tmp;
			}
		      }
		      if(r->flags & FW_POINTER && r->flags & FW_CHAR)
			resolve_string(run, r);
		      else if(r->flags & FW_POINTER)
			resolve_pointer(run, r);
		      else if(r->flags & FW_ENUM)
			resolve_enum(run, r);
		      else if(r->flags & FW_UNION)
			resolve_struct(run,r,0);
		      else if(r->flags &FW_STRUCT)
			resolve_struct(run, r,1);
		    }
		  }
		}
		
		rc = ptrace(PTRACE_SINGLESTEP, run->child_pid, 0, 0);
		if( rc != 0)
		  debug_printf("Error singlestepping: %s\n", strerror(errno));
		
		rc_pid = waitpid(run->child_pid, &child_status, 0);
		copy_changes(run->child_pid, (unsigned long *) old_buf, (unsigned long *) new_buf, address, sz);
		rc = ptrace(PTRACE_CONT, run->child_pid, 0, 0);
		if( rc != 0)
		  debug_printf("Error resuming process: %s\n", strerror(errno));
		    			        
		rc_pid = waitpid(run->child_pid, &child_status, 0);
		if(WIFSIGNALED(child_status))
		  debug_printf("Child process terminated by signal %d\n", WTERMSIG(child_status));        
		if(WIFEXITED(child_status)){
		  return run;
		}
	      }
	    }
	  }
	  // check the next symbol in this compilation unit
	  rc = dwarf_siblingof(run->dwarf_ptr, child_die, &child_die, &err);
	  if (rc == DW_DLV_ERROR){
	    return 0;
	  }
	  else if (rc == DW_DLV_NO_ENTRY)
	    break; /* done */
	} //end child die loop
      }

      // check the next source file
      rc=dwarf_next_cu_header(run->dwarf_ptr,NULL,NULL,NULL,NULL,&next_cu_header, &err);
      if(rc != DW_DLV_OK) // either out of entries or something's wrong
	break;
    } // end source file loop

    debug_printf("Unable to find function %s\n", func_name);
    exit(-1);
  } // end parent process code
  
  return run;
}

static funcwatch_param *resolve_string(funcwatch_run *run, funcwatch_param *p){
  if(p->value == 0){
    // NULL pointer, do not need to resolve.
    // return the current p
    return p;
  }

  
  long rc = 0;
  void * address = (void *) p->value;
  
  funcwatch_param *pointee = (funcwatch_param *)malloc(sizeof(funcwatch_param));
  funcwatch_param_initialize(pointee);

  if(p->flags & FW_CHAR) {
    int sz = 128;
    char *buf = malloc(sz);
    struct iovec liovec, riovec;
    liovec.iov_base = buf;
    liovec.iov_len = sz;
    riovec.iov_base = address;
    riovec.iov_len = sz;
    rc = process_vm_readv(run->child_pid, &liovec, 1, &riovec, 1, 0);
    if(rc != sz) {
      debug_printf("Error resolving pointer for variable %s:%s\n", p->name, strerror(errno));
      p->flags |= FW_INVALID;
    }
    rc = (int) memchr(buf, 0, sz);
    while(rc == -1)	 {
      sz *= 2;
      buf = realloc(buf, sz);
      liovec.iov_base = buf;
      liovec.iov_len = sz;
      riovec.iov_base = address;
      riovec.iov_len = sz;
      rc = process_vm_readv(run->child_pid, &liovec, 1, &riovec, 1, 0);
      if(rc != sz) {
	debug_printf("Error resolving pointer for variable %s:%s\n", p->name, strerror(errno));
	p->flags |= FW_INVALID;
      }
      rc = (int) memchr(buf, 0, sz);
    }
    p->value = (long) buf;
    p->size = strlen(buf);
    return;
  }
  
  /*
   * for int*, char* pointers, this function allocate a local memory block, and transfer the data from 
   * the remote process (target function) to the local memory block.
   * the pointer's value is the address of the local memory block.
   */
  /* 
  if(p->flags & FW_INT || p->flags & FW_FLOAT) { 
    struct iovec liovec, riovec;
    p->value = (long) malloc(p->size);
    liovec.iov_base = (void *) (p->value);
    liovec.iov_len = p->size;
    riovec.iov_base = address;
    riovec.iov_len = p->size;
    rc = process_vm_readv(run->child_pid, &liovec, 1, &riovec, 1, 0);
    if(rc != p->size) {
      debug_printf("Error resolving pointer for variable %s:%s\n", p->name, strerror(errno));
    }
    
    if(p->flags & FW_SIGNED){
      if(p->size == 2 && p->flags & FW_INT){
	short *ptr = (short *)p->value;
	p->value = (long)malloc(sizeof(long));
	*(long *)(p->value) = *ptr;
      }else if(p->size ==sizeof(long long) && p->flags & FW_INT){
	long long *ptr = (long long *)p->value;
	p->value = (long)malloc(sizeof(long));
	*(long *)(p->value) = *ptr;
	debug_printf("Warning: %s\n", "Cast a long long to a long.");
      }
    }
    else{
      if(p->size == 2 && p->flags & FW_INT){
	unsigned short *ptr = (unsigned short *)p->value;
	p->value = (long)malloc(sizeof(long));
	*(unsigned long *)(p->value) = *ptr;
      }else if(p->size ==sizeof(long long) && p->flags & FW_INT){
	unsigned long long *ptr = (unsigned long long *)p->value;
	p->value = (long)malloc(sizeof(long));
	*(unsigned long *)(p->value) = *ptr;
	debug_printf("Warning: %s\n", "Cast an unsigned long long to an unsigned long.");
      }
    }
    
    if(p->size ==sizeof(double) && p->flags & FW_FLOAT){
      double *ptr = (double *)p->value;
      p->value = (long)malloc(sizeof(long));
      *(float *)(p->value) = *ptr;
      debug_printf("Warning: %s\n", "Cast a double to a float.");
    }else if(p->size ==sizeof(long double) && p->flags & FW_FLOAT){
      long double *ptr = (long double *)p->value;
      p->value = (long)malloc(sizeof(long));
      *(float *)(p->value) = *ptr;
      debug_printf("Warning: %s\n", "Cast a long double to a float.");
    }
    return;
  }
  
  else {
    debug_printf("Error: pointer %s is not going to be resolved.\n", p->value);
    p->flags |= FW_INVALID;
    return;
  }
  */  
}

static funcwatch_param *resolve_pointer(funcwatch_run *run, funcwatch_param *p) {
  if(p->value == 0){
    // NULL pointer, do not need to resolve.
    // return the current p
    return p;
  }

  
  long rc = 0;
  void * address = (void *) p->value;
  funcwatch_param *pointee = (funcwatch_param *)malloc(sizeof(funcwatch_param));
  funcwatch_param_initialize(pointee);

  p->next = pointee;
  pointee->name = malloc(strlen(p->name) + 2);
  pointee->name[0] = '*';
  strcpy(&(pointee->name[1]), p->name);
  pointee->name[strlen(p->name) + 1] = '\0';
  pointee->func_name = strcpy_deep(p->func_name);
  pointee->call_num = p->call_num;
  pointee->addr = p->value;
  
  get_type_info_from_parent_type_die(run->dwarf_ptr, p->type_die, pointee);
  get_value_from_remote_process_inner(pointee, run->child_pid);

  funcwatch_param *lastEvolvedParam = pointee;
  if(pointee->flags & FW_POINTER && pointee->flags & FW_CHAR)
    lastEvolvedParam = resolve_string(run, pointee);
  else if(pointee->flags & FW_POINTER)
    lastEvolvedParam = resolve_pointer(run, pointee);
  else if(pointee->flags & FW_ENUM)
    lastEvolvedParam = resolve_enum(run, pointee);
  else if(pointee->flags & FW_UNION)
    lastEvolvedParam = resolve_struct(run, pointee, 0);
  else if(pointee->flags &FW_STRUCT)
    lastEvolvedParam = resolve_struct(run, pointee, 1);
  return lastEvolvedParam;

}

/*
 * is_resolve_pointer: 
 *   if it's true, then resolve pointers, struct pointers, enum pointers, etc.
 *   if it's false, then do not resolve any pointers
 * this indicator helps when we meet a union.
 * inside a union, we are not sure whether a pointer is valid or not.
 */
static funcwatch_param *resolve_struct(funcwatch_run *run, funcwatch_param *p, int is_resolve_pointer) {
  int level = p->struct_level;
  funcwatch_param *tmp = p;
  Dwarf_Die child_die;
  Dwarf_Error err;
  int rc = 0;
  if((p->flags & FW_STRUCT) || (p->flags & FW_UNION))
    rc=dwarf_child(p->type_die, &child_die, &err); // child_die is a tag_member
  else
    debug_printf("Error: %s\n", "resolve_struct only deals with struct or union variables.");
  
  if(rc != DW_DLV_OK){
    debug_printf("Error:%s ", "cannot get a child type/var die from a struct/union var die.");
    if(rc == DW_DLV_NO_ENTRY)
      debug_printf("%s", "struct/union var die does not have child type/var die.");
  
    if(rc ==DW_DLV_ERROR) 
      debug_printf("%s", dwarf_errmsg(err));

    debug_printf("%s\n", " ");
    exit(-1);
  }  

  /*
  funcwatch_param *non_pointer = getFirstNonPointer(p);
  void *struct_base = non_pointer->addr;
  if(struct_base == 0){
    debug_printf("Info: %s\n","NULL struct pointers");
    return p;
    }*/
  void *struct_base = p->addr;
  
  while(1) {
    tmp->next = malloc(sizeof(funcwatch_param));
    tmp = tmp->next;
    funcwatch_param_initialize(tmp);
    
    char *var_name;
    Dwarf_Attribute attr;  
    dwarf_diename(child_die, &var_name, &err);
    
    long len = strlen(var_name)+strlen(p->name)+2;
    tmp->name = (char *) malloc(len);
    strcpy(tmp->name, p->name);
    tmp->name[strlen(p->name)] = '.';
    strcpy(tmp->name+1+strlen(p->name), var_name);
    tmp->func_name = p->func_name;
    tmp->call_num = p->call_num;
    tmp->value = 0;
    tmp->struct_level = level + 1;
    tmp->var_die = child_die;
    get_type_info_from_var_die(run->dwarf_ptr, child_die, tmp);
    Dwarf_Unsigned offset;

    // Get the offset
    if(p->flags & FW_UNION){
      offset = 0;
    }else if(p->flags & FW_STRUCT){
      rc = dwarf_attr(tmp->var_die, DW_AT_data_member_location, &attr, &err);
      if(rc != DW_DLV_OK){
	debug_printf("Error: %s ", "cannot get location of a struct member.");
	if(rc == DW_DLV_ERROR)
	  debug_printf("%s", dwarf_errmsg(err));
	debug_printf("%s\n", " ");
	exit(-1);
      }
      rc = dwarf_formudata(attr, &offset, &err);
      // if the struct variable die is a pointer struct type
      // the base address should be the pointer's value not the pointer's address
    }
    
    tmp->addr = struct_base + offset;
    if(!(tmp->flags & FW_INVALID)){ 
      /* After this if statement, the value should be set! */    
      if(tmp->flags & FW_STRUCT && !(tmp->flags & FW_POINTER))
	tmp->value = tmp->addr;
      else if(tmp->flags & FW_UNION && !(tmp->flags & FW_POINTER))
	tmp->value = tmp->addr; // if tmp is a union, we need the addr to resolve.
      else
	get_value_from_remote_process_inner(tmp, run->child_pid);
      
      //debug_printf("var_name: %s\n", var_name);
      if(tmp->flags & FW_POINTER && tmp->flags & FW_CHAR && is_resolve_pointer ) {
	resolve_string(run, tmp);
	tmp = get_end_of_list(tmp);
      }
      else if(tmp->flags & FW_POINTER && is_resolve_pointer ) {
	resolve_pointer(run, tmp);
	tmp = get_end_of_list(tmp);
      }
      else if(tmp->flags & FW_STRUCT
	      && (is_resolve_pointer || !(tmp->flags & FW_POINTER))) {
	if( level < MAX_RESOLVE_DEPTH){
	  if(tmp->type != NULL && p->type != NULL){
	    if(strcmp(tmp->type, p->type) != 0) {
	      resolve_struct(run, tmp, is_resolve_pointer);
	      // skip all the members in struct tmp
	      tmp = get_end_of_list(tmp);
	    }
	  }else{
	    resolve_struct(run, tmp, is_resolve_pointer);
	    // skip all the members in struct tmp
	    tmp = get_end_of_list(tmp);
	  }
	}
      }
      else if(tmp->flags & FW_UNION
	      && (is_resolve_pointer || !(tmp->flags & FW_POINTER))){
	resolve_struct(run, tmp, 0);
	tmp = get_end_of_list(tmp);
      }
      else if(tmp->flags & FW_ENUM
	      && (is_resolve_pointer || !(tmp->flags & FW_POINTER))) {
	resolve_enum(run, tmp);
	tmp = get_end_of_list(tmp);
      }
    }
    // go to the next member
    Dwarf_Bool b;
    // because child_die is assigned to tmp->tmp_die
    // we do not free child_die here.
    rc=dwarf_siblingof_b(run->dwarf_ptr, child_die, b, &child_die, &err);
    if (rc == DW_DLV_NO_ENTRY) {
      // done
      return tmp;
    }
    else if(rc !=DW_DLV_OK) {
      debug_printf("Error:%s\n", dwarf_errmsg(err));
      exit(-1);
    }
  } // end child loop
  
  return p; //BUG!!!!!!!!!!!!!!!!!TBC
}

funcwatch_param *resolve_enum(funcwatch_run *run, funcwatch_param *p) {
  Dwarf_Die child_die;
  Dwarf_Error err;
  int rc=dwarf_child(p->type_die, &child_die, &err);
  if(rc !=DW_DLV_OK) {
    debug_printf("Error:%s\n", dwarf_errmsg(err));
    exit(-1);
  }
  long p_value = p->value;
  
  /*
   * if it is a enum pointer, we remove the pointer flag from the parameter
   * assign the value of enum directly to the parameter
   */
  /*
  if(p->flags & FW_POINTER){
    if( p->value != 0){
      get_value_from_remote_process_inner(p, run->child_pid);
      
      p->flags &= ~FW_POINTER;
    }else{
      debug_printf("Info: %s\n", "NULL enum pointer.");
      return p;
    }
  }
  */
  
  while(1) {
    char *var_name;
    Dwarf_Unsigned val;
    Dwarf_Attribute attr;
    dwarf_diename(child_die, &var_name, &err);
    rc = dwarf_attr(child_die,DW_AT_const_value,&attr,&err);
    rc = dwarf_formsdata(attr, &val, &err);
    if(val == p->value) {
      p->value = (long) var_name;
      //debug_printf("Resolved enum value %s for variable %s\n", (char *) p->value, p->name);
      return;
    }
    // go to the next member
    rc=dwarf_siblingof(run->dwarf_ptr, child_die, &child_die, &err);
    if (rc == DW_DLV_NO_ENTRY) {
      //debug_printf("Warning: unable to resolve enum %s for variable %s\n", p->type, p->name);
      p->value = (long) "<invalid>";
      return p;  // TBC
    }
  } // end child loop
}

void copy_changes(pid_t pid, unsigned long *old_buf, unsigned long *new_buf, unsigned  long address, size_t size) {
  size_t chunk_size = sizeof(void *);
  for(int i = 0; i < (size/chunk_size); ++i) {
    if(old_buf[i] != new_buf[i]) {
      unsigned long  addr = address+(chunk_size*i);
      unsigned long data = new_buf[i];
      long rc = ptrace(PTRACE_POKEDATA, pid, (void *) addr, (void *) data);
      if( rc != 0) {
	debug_printf("Error setting breakpoint: %s\n", strerror(errno));
	exit(-1);
      }
    }
  }
}
