#include <stdio.h>
#include <stdlib.h>
#include <dirent.h> 
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <limits.h>

#include "vector.h"

// String utility functions - definition
int ends_with(const char *str, const char *suffix)
{
  if (!str || !suffix)
    return 0;
  size_t lenstr = strlen(str);
  size_t lensuffix = strlen(suffix);
  if (lensuffix >  lenstr)
    return 0;
  return strncmp(str + lenstr - lensuffix, suffix, lensuffix) == 0;
}

int starts_with(const char *str, const char *prefix)
{
  if (!str || !prefix)
    return 0;
  size_t lenstr = strlen(str);
  size_t lenprefix = strlen(prefix);
  if (lenprefix >  lenstr)
    return 0;
  return strncmp(str, prefix, lenprefix) == 0;
}

char *strcpy_deep(const char *str){
  size_t str_len = strlen(str) + 1;
  char *ret = malloc(sizeof(char)*str_len);
  strcpy(ret, str);
  return ret;
}

// Dynamic Array utility functions - definition
void vector_init(Vector *vector) {
  // initialize size and capacity
  vector->size = 0;
  vector->capacity = VECTOR_INITIAL_CAPACITY;
  // allocate memory for vector->data
  vector->data = malloc(sizeof(void *) * vector->capacity);
}

void vector_append(Vector *vector, void *data_ptr) {
  // make sure there's room to expand into
  vector_double_capacity_if_full(vector);
  // append the value and increment vector->size
  vector->data[vector->size++] = data_ptr;
}

void *vector_get(Vector *vector, int index) {
  if (index >= vector->size || index < 0) {
    printf("Index %d out of bounds for vector of size %d\n", index, vector->size);
    exit(EXIT_FAILURE);
  }
  return vector->data[index];
}

void *vector_last(Vector *vector){
  if(vector->size == 0)
    return NULL;
  return vector->data[vector->size-1];
}

void *vector_remove_last(Vector *vector){
  if(vector->size == 0)
    return NULL;
  vector->size --;
  return vector->data[vector->size];
}

void *vector_remove_at(Vector *vector, int index){
  if(vector->size == 0 || index < 0 || index > (vector->size -1))
    return NULL;
  
  void *ret_data = vector->data[index];
  if(index != vector->size - 1)
    memcpy(vector->data+index*sizeof(void *),
	   vector->data+(index+1)*sizeof(void *), sizeof(void *)*(vector->size -1-index));

  vector->size --;
  return ret_data;
}

void vector_set(Vector *vector, int index, void *data_ptr) {
  // zero fill the vector up to the desired index
  while (index >= vector->size) {
    vector_append(vector, 0);  }
  // set the value at the desired index
  vector->data[index] = data_ptr;
}

void vector_double_capacity_if_full(Vector *vector) {
  if (vector->size >= vector->capacity) {
    // double vector->capacity and resize the allocated memory accordingly
    vector->capacity *= 2;
    vector->data = realloc(vector->data, sizeof(int) * vector->capacity);
  }
}

void vector_inner_free(Vector *vector) {
  for(int i=0; i< vector->size; i++){
    void *data_p = vector_get(vector, i);
    free(data_p);
  }
  free(vector->data);
}
