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
  for (char **vars = qc->env; *vars != 0; vars++)
  {
    free(*vars);
  }
  free(qc->env);
  free(qc);
}

void updateCWD(struct QuashContext* qc)
{
  free(qc->cwd);
  cwd = activeDirectory(qc)
}