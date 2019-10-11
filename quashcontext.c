#include "quashcontext.h"

struct QuashContext* contextCtor(char**env)
{
    struct QuashContext* returnContextPtr = malloc(sizeof(struct QuashContext));
    returnContextPtr->cwd = activeDirectory(env);
    return returnContextPtr;
}