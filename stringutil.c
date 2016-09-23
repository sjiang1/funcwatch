#include <stdio.h>
#include <stdlib.h>
#include <dirent.h> 
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <limits.h>

#include "stringutil.h"

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

// buffer must have length >= sizeof(int) + 1
// Write to the buffer backwards so that the binary representation
// is in the correct order i.e.  the LSB is on the far right
// instead of the far left of the printed string
char *int2bin(int a, char *buffer, int buf_size) {
  buffer += (buf_size - 1);

  for (int i = 31; i >= 0; i--) {
    *buffer-- = (a & 1) + '0';

    a >>= 1;
  }

  return buffer;
}

int count_lines(char *str){
  int stringLength = strlen(str);
  if(stringLength < 1){
    return 0;
  }
  
  int line_count = 1;
  for(int i = 0; i < stringLength; i++){
    char ch = str[i];
    if(ch == '\n'){
      line_count ++;
    }
  }

  return line_count;
}
