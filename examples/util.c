#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"

char* split_string (
  const char ***arr,
  const char *str,
  const char *delims
)
{
  char *newstr = strdup (str);
  char *token;
  
  size_t tokidx = 0;
  
  *arr = NULL;
  token = strtok (newstr, delims);
  
  do {
  split_begin:
    *arr = realloc (*arr, sizeof (const char**) * (tokidx + 1));
    if (!*arr)
      goto drop;
    
    (*arr)[tokidx++] = token;
    
    if (token)
    {
      token = strtok (NULL, delims);
      
      if (!token) // put the NULL.
        goto split_begin;
    }
  } while (token != NULL);

  // NOTE: don't free (ref'd by arr).
  return newstr;

drop:
  free (newstr);
  *arr = NULL;

  return NULL;
}
