#ifndef _DYNSTRING_H
#define _DYNSTRING_H

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h> 
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <limits.h>

// Dynamic Char-Array (String) utility functions - declaration
#define STRING_INITIAL_CAPACITY 100
typedef struct {
  int size;      // slots used so far
  int capacity;  // total available slots
  char *text;     // a char array
} DynString; 

void dynstring_init(DynString *dynString);
void dynstring_append(DynString *dynString, char *str);
void dynstring_double_capacity_if_full(DynString *dynString, int toAppend);
void dynstring_inner_free(DynString dynString);

#endif // _DYNSTRING_H
