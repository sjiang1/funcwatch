#include <stdio.h>
#include <unistd.h>
int foo(long i, long j) { 
printf("foo: i: %ld, address: %p\n", i, &i);
printf("foo: j: %ld, address: %p\n", j, &j);
return i * j; 
}


short foo_short(short i, short j) { 
printf("foo_short: i: %hd, address: %p\n", i, &i);
printf("foo_short: j: %hd, address: %p\n", j, &j);
return i * j; 
}

float foo_float(float i, float j) {
  printf("foo_float: i: %g, address: %p\n", i, &i);
  printf("foo_float: j: %g, address: %p\n", j, &j);
  float ret =  i*j;
  printf("foo_float: ret: %g\n", ret); 
  return ret; 
}
void * foo_ptr(long i, long j) { 
printf("foo_ptr: i: %ld, address: %p\n", i, &i);
printf("foo_ptr: j: %ld, address: %p\n", j, &j);
return (void *) (i * j); 
}

char * foo_str(long i, long j) { 
printf("foo_str: i: %ld, address: %p\n", i, &i);
printf("foo_str: j: %ld, address: %p\n", j, &j);
return "abcdefg"; 
}
typedef enum{
a, b, c} my_enum;

void foo_enum(my_enum e) {}
struct my_struct {
long  i;
float  j;
};
struct my_struct * foo_struct(struct my_struct *f) { 
	printf("foo_struct: f: %p, address:%p\n", f, &f);
	printf("foo_struct: i: %ld, address: %p\n", f->i, &f->i);
printf("foo_struct: j: %g, address: %p\n", f->j, &f->j);
return f; 
}


int main(int argc, char *argv[]) {
  printf("%d args:\n", argc-1);
  for(int i = 1; i < argc; ++i)
    printf("arg %d:%s\n", i, argv[i]);
  
  //printf("In main. Pid:%d\n", getpid());
  struct my_struct f;
  my_enum e = a;
  
  for(long i = 0; i < 5; ++i) {
    foo(i, i+1);
    foo_float(i, i+1);
    foo_ptr(i, i+1);
    foo_str(i, i+1);
    f.i = i+1;
    f.j = i+1;
    foo_struct(&f);
    foo_short(i, i+1);
    foo_enum(e);
  }
  
  return 0;

}

