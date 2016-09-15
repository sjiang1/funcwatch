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
