#ifndef _UTIL_H
#define _UTIL_H

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h> 
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <limits.h>

// String utility functions - declaration
int ends_with(const char *str, const char *suffix);
int starts_with(const char *str, const char *prefix);
char *strcpy_deep(const char *str);

// Dynamic Array utility functions - declaration
#define VECTOR_INITIAL_CAPACITY 100
typedef struct {
  int size;      // slots used so far
  int capacity;  // total available slots
  void **data;     // array of pointers we're storing
} Vector; 

void vector_init(Vector *vector);
void vector_append(Vector *vector, void *data_ptr);
void *vector_get(Vector *vector, int index);
void *vector_last(Vector *vector);
void *vector_remove_last(Vector *vector); // return the last element, and remove it from the vector
void *vector_remove_at(Vector *vector, int index); // return the element, and remove it from the vector
void vector_set(Vector *vector, int index, void *data_ptr);
void vector_double_capacity_if_full(Vector *vector);
void vector_inner_free(Vector *vector);

#endif 
