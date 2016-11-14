#include <stdio.h>
#include <stdlib.h>
#include "vector.h"

int main(int argc, char *argv[]) {  
  Vector vector;
  vector_init(&vector);

  // fill it up with 150 arbitrary values
  // this should expand capacity up to 200
  int i;
  for (i = 2; i > 0; i--) {
    int *data = (int *) malloc(sizeof(int));
    *data = i;
    vector_append(&vector, data);
  }

  // we're all done playing with our vector,
  // so free its underlying data array
  vector_inner_free(&vector);
}
