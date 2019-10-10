#ifndef PATH_H
#define PATH_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "quashutils.h"

// given envp from main() returns malloced string of current working directory
char* initial_cwd(char** environ);



#endif
