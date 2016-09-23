#ifndef _STRINGUTIL_H
#define _STRINGUTIL_H

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
char *int2bin(int a, char *buffer, int buf_size);
int count_lines(char *str);

#endif //_STRINGUTIL_H
