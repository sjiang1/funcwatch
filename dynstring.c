#include <stdio.h>
#include <stdlib.h>
#include <dirent.h> 
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <limits.h>

#include "dynstring.h"

void dynstring_init(DynString *dynString){
  // initialize size and capacity
  dynString->size = 0;
  dynString->capacity = STRING_INITIAL_CAPACITY;
  // allocate memory for dynString->text
  dynString->text = malloc(sizeof(char) * dynString->capacity);
  (dynString->text)[0] = '\0';
}

void dynstring_append(DynString *dynString, char *str){
  // make sure there's room to expand into
  dynstring_double_capacity_if_full(dynString);
  // append the text
  strcat(dynString->text,str);
}

void dynstring_double_capacity_if_full(DynString *dynString, int toAppend){
  if (dynString->size + toAppend >= dynString->capacity) {
    // double vector->capacity and resize the allocated memory accordingly
    dynString->capacity *= 2;
    dynString->text = realloc(dynString->text, sizeof(char) * dynString->capacity);
  }
}

void dynstring_inner_free(DynString dynString){
  free(dynString->text);
}
