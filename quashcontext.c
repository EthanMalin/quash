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

void printAllEnvVars(struct QuashContext* qc)
{
  for (char **vars = qc->env; *vars != 0; vars++)
  {
    char *thisVar = *vars;
    printf("%s\n", thisVar);    
  }
}

void updateCWD(struct QuashContext* qc)
{
  free(qc->cwd);
  printf("Attempting to update CWD\n");
  qc->cwd = activeDirectory(qc->env);
}

void updateCWD_(struct QuashContext* qc, char* newPath)
{
  free(qc->cwd);
  printf("Attempting to update CWD\n");
  qc->cwd = newPath;
}