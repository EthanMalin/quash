#include "quashcontext.h"

struct QuashContext* contextCtor(char** _env)
{
  struct QuashContext* returnContextPtr = malloc(sizeof(struct QuashContext));
  returnContextPtr->env = _env;
  returnContextPtr->cwd = activeDirectory(_env);
  size_t i = 0;
  while (returnContextPtr->env[i] != NULL)
  {
    char** temp = split(returnContextPtr->env[i], "=", 3);
    if (strcmp(temp[0], "PATH") == 0)
    {
      memmove(returnContextPtr->env[i], returnContextPtr->env[i]+5, strlen(returnContextPtr->env[i]));
      returnContextPtr->paths = split(returnContextPtr->env[i], ":", 20);
      break;
    }
    i++;
  }
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

void printAllPaths(struct QuashContext* qc)
{
  for (char **vars = qc->paths; *vars != 0; vars++)
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