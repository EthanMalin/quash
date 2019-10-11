#ifndef QUASH_CONTEXT_H
#define QUASH_CONTEXT_H

#include <stdlib.h>

#include "path.h"

struct QuashContext
{
  char* cwd;
};

// Allocates space for and creates the QuashContext struct.
// Sets initial cwd. Does not deallocate memory.
struct QuashContext* contextCtor(char**env);

// Deallocates all memory for which the given QuashContext
// is responsible for, then deallocates it as well.
void contextDtor(struct QuashContext* qc);

#endif