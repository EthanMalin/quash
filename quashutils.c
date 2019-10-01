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
 *
 *   returns -- array of dynamically allocated strings
 */
char** split(char *input, char *delim, int maxSections) {
  char **res;
  res = malloc(maxSections * sizeof(char*));
  for(int i = 0; i < maxSections; i++) { res[i] = NULL; }
  
  // feed strtok and get first ptr
  char *section = strtok(input, delim);
  int i = 0;
  while (section != NULL) {
    res[i] = malloc(strlen(section));
    strcpy(res[i], section);
    i++;

    // if we have gotten too many inputs, free and abort, returning NULL
    if (i == maxSections) {
      printf("Split input too long! Aborting with input: %s\n", input);
      for (int j = 0; j < maxSections; j++) { free(res[j]); }
      free(res);
      return NULL;
    } else {
      // get next string from strtok
      section = strtok(NULL, delim);
    }

    
  }
  return res;
}


/*
trimEnds -- trims whitespace from start and end of string
  input -- string to trim
  return -- new string without whitespace on the ends
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
 
