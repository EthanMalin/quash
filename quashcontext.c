#include "quashcontext.h"

struct QuashContext* contextCtor(char** _env)
{
  struct QuashContext* returnContextPtr = malloc(sizeof(struct QuashContext));
  returnContextPtr->env = _env;
  returnContextPtr->cwd = activeDirectory(_env);
  return returnContextPtr;
}

void contextDtor(struct QuashContext* qc)
{
  free(qc->cwd);
  free(qc);
}

void updateCWD(struct QuashContext* qc)
{
  free(qc->cwd);
  qc->cwd = activeDirectory(qc->env);
}