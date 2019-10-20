#ifndef QUASH_UTILS_H
#define QUASH_UTILS_H

#include <stdbool.h>

/* splits input on delim, copying data into a newly malloced segment */
char** split(char *input, char *delim, int maxSections);
/* returns substring of input, not to be freed */
char* trimEnds(char *input);
char* trimEndsCopy(const char *input);
bool isSpace(char input);

char* concat(char* str1, char* str2);

#endif
