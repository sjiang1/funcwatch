#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"
#include "commandhandle.h"

char *trimwhitespace(char *str);

void read_functionnames(Vector *funcnames, const char *filename){
  FILE* fp;
  char buffer[500];
  char *funcname = NULL;

  fprintf(stderr, "open file: %s\n", filename);
  fp = fopen(filename, "r");
  while(fgets(buffer, 500, (FILE*) fp)) {
    funcname = trimwhitespace(buffer);
    size_t funcname_len = strlen(funcname);
    if (funcname_len <= 0)
      continue;
    
    char *data_to_save = (char *) malloc((funcname_len + 1) * sizeof(char));
    strncpy(data_to_save, funcname, funcname_len + 1); 
    vector_append(funcnames, data_to_save);
    
  }

  fclose(fp);
  return;
}

// Note: This function returns a pointer to a substring of the original string.
// If the given string was allocated dynamically, the caller must not overwrite
// that pointer with the returned value, since the original pointer must be
// deallocated using the same allocator with which it was allocated.  The return
// value must NOT be deallocated using free() etc.
char *trimwhitespace(char *str)
{
  char *end;

  // Trim leading space
  while(isspace(*str)) str++;

  if(*str == 0)  // All spaces?
    return str;

  // Trim trailing space
  end = str + strlen(str) - 1;
  while(end > str && isspace(*end)) end--;

  // Write new null terminator
  *(end+1) = 0;

  return str;
}
