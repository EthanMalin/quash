#ifndef PATH_H
#define PATH_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "quashutils.h"

// given envp from main() returns malloced string of current working directory
// The callee must free this memory.
char* activeDirectory(char** env);



#endif
