#ifndef QUASH_UTILS_H
#define QUASH_UTILS_H

#include <stdbool.h>

/* splits input on delim, copying data into a newly malloced segment */
char** split(char *input, char *delim, int maxSections);
/* returns substring of input, not to be freed */
char* trimEnds(char *input);
char* trimEndsCopy(const char *input);
bool isSpace(char input);

// Concatenates two strings and returns the result.
char* concat(char* str1, char* str2);

// Removes end of string following the final instance of "/"
// This is particularly optomized for path operations.
void deleteEnd (char* myStr);
#endif
