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
 *   returns -- array of dynamically allocated strings, padded with NULL
 */
char** split(char *input, char *delim, int maxSections) {
  char **res;
  char *temp = malloc(strlen(input) + 1);
  strncpy(temp, input, strlen(input));
  temp[strlen(input)] = '\0';

  res = malloc(maxSections * sizeof(char*) + 1); // +1 so there is always a NULL at the end
  for(int i = 0; i < maxSections+1; i++) { res[i] = NULL; }
  // feed strtok and get first ptr
  char *section = strtok(temp, delim);
  int i = 0;

  while (section != NULL) {
    res[i] = malloc(strlen(section));
    strcpy(res[i], section);
    i++;

    // if we have gotten too many temps, free and abort, returning NULL
    if (i == maxSections) {
      printf("Split temp too long! Aborting with temp: %s\n", temp);
      for (int j = 0; j < maxSections; j++) { free(res[j]); }
      free(res);
      free(temp);
      return NULL;
    } else {
      // get next string from strtok
      section = strtok(NULL, delim);
    }

    
  }
  free(temp);
  return res;
}


/*
trimEnds -- trims whitespace from start and end of string
  input -- string to trim
  return -- subsstring without whitespace on the ends
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

char* trimEndsCopy(const char *input) {
  char *res;
  const char *end = input + strlen(input) - 1;
  const char *beg = input;
  // seek beg to first non-space
  while (isSpace(*beg)) { beg++; }
  if (*beg == '\0') { return NULL; }

  // seek end to last non-space
  while (end >= beg && isSpace(*end)) { end--; }
  res = malloc(end - beg + 2);
  int i = 0;
  for (; i < end - beg + 1; i++) {
    res[i] = beg[i];
  }
  res[i] = '\0';
  return res;
}

char* concat(char* str1, char* str2) {
      char * str3 = (char *) malloc(1 + strlen(str1)+ strlen(str2) );
      strcpy(str3, str1);
      strcat(str3, str2);
      free(str1);
      // free(str2);
      return str3;
}

void deleteEnd (char* myStr) {

  // printf ("%s\n", myStr);
  char *del = &myStr[strlen(myStr)];
  // if(*del == '/') {
  //   del--;
  // }
  // printf("Del %s\n", del);
  del--;
  del--;
  // while (del > myStr && *del != '/')
  // printf("Del %s\n", del);
  while (del > myStr && *del != '/')
      // printf("Iterate\n");
      del--;

  if (*del== '/')
      *del= '\0';

  // printf("%s\n", myStr);
  return;
}