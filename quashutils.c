#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "quashutils.h"

bool isSpace(char input) {
  return input == ' ';
}

/*
 * split -- splits strings into array of strings based on character
 *   input -- string to split
 *   delim -- character to split on
 *   maxSections -- largest allowed number of sections to split into
 *   maxStringSize -- longest possible section length
 *
 *   returns -- array of dynamically allocated strings
 */
char** split(char *input, char *delim, int maxSections, int maxStringSize) {
  char **res;
  res = malloc(maxSections * sizeof(char*));
  // initialize to NULL
  for(int i = 0; i < maxSections; i++) { res[i] = NULL; } 
  // feed strtok and get first ptr
  char *ptr = strtok(input, delim);
  int i = 0;
  while (ptr != NULL) {
    // if we have gotten too many inputs, free and abort, returning NULL
    if (i == maxSections) {
      printf("Pipeline too long! Aborting...\n");
      for (int i = 0; i < maxSections; i++) { free(res[i]); }
      free(res);
      return NULL;
    }
    // append to res
    res[i] = malloc(maxStringSize);
    strcpy(res[i], ptr);
    i++;
    // get next string from strtok
    ptr = strtok(NULL, delim);
  }
  return res;
}


/*
trimEnds -- trims whitespace from start and end of string
  input -- string to trim
  return -- substring of input 
*/
char* trimEnds(char *input) {
  char *end;
  // interate through string until we don't see a space
  while (isSpace(*input)) { input++; }
  // string was all spaces return "\0"
  if (*input == '\0') { return input; }
  // point to end
  end = input + strlen(input) - 1;
  // do the same thing backwards
  while (end > input && isSpace(*end)) { end--; }
  end[1] = '\0';
  return input;
}
 
