#ifndef QUASH_CONTEXT_H
#define QUASH_CONTEXT_H

#include <stdlib.h>

#include "path.h"

struct QuashContext
{
  char* cwd;
  char** env;
  char** paths;
};

// Allocates space for and creates the QuashContext struct.
// Sets initial cwd. Does not deallocate memory.
struct QuashContext* contextCtor(char** _env);

// Deallocates all memory for which the given QuashContext
// is responsible for, then deallocates it as well.
void contextDtor(struct QuashContext* qc);

// Prints all elements inside env to stdout.
void printAllEnvVars(struct QuashContext* qc);

// Updates the current working directory of the context
// with the current PWD from envp
void updateCWD(struct QuashContext* qc);

// Updates the current working directory of the context
// to the path passed in.
void updateCWD_(struct QuashContext* qc, char* newPath);
#endif