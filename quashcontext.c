#include "quashcontext.h"

struct QuashContext* contextCtor(char** _env)
{
  struct QuashContext* returnContextPtr = malloc(sizeof(struct QuashContext));
  returnContextPtr->env = _env;
  printf("calling activeDirectory\n");
  returnContextPtr->cwd = activeDirectory(_env);
  printf("got activeDirectory\n");
  size_t i = 0;
  while (returnContextPtr->env[i] != NULL)
  {
    printf("splitting %s\n", returnContextPtr->env[i]);
    char** temp = split(returnContextPtr->env[i], "=", 3);
    if (temp == NULL) {
      i++;
      continue;
    }
    printf("split\n");
    if (strcmp(temp[0], "PATH") == 0)
    {
      printf("detected PATH\n");
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
  // printf("Attempting to update CWD\n");
  qc->cwd = activeDirectory(qc->env);
}

void updateCWD_(struct QuashContext* qc, char* newPath)
{
  free(qc->cwd);
  // printf("Attempting to update CWD\n");
  qc->cwd = newPath;
}

char* getFilePath(struct QuashContext* qc, char* fileName)
{
  // printf("fileName %s\n", fileName);
  char* fileName_ = malloc(strlen(fileName) + 1);
  strcpy(fileName_, fileName);
  // printf("Inside getFilePath\n");
  // Check current path first.
  char* path_;
  // printf("fileName_ %s\n", fileName_);
  size_t len = strlen(qc->cwd);
  path_ = malloc(strlen(qc->cwd) + 2);
  strcpy(path_, qc->cwd);
  // printf("Path_ %s\n", path_);
  path_[len] = '/';
  path_[len + 1] = '\0';
  // printf("Path_ %s\n", path_);
  fflush(NULL);
  // printf("fileName_ %s\n", fileName_);
  char* pathAndFile_ = concat(path_, fileName_);
  printf("Final pathAndFile_ for cwd: %s\n", pathAndFile_);
  free(path_);
  if(access(pathAndFile_, F_OK) != -1 )
  {
    // file exists
    printf("DEBUG: file found in current directory!\n");
    return(pathAndFile_);
  }
  else
  {
    printf("DEBUG: file _not_ found in current directory!\n");
  }
  for(size_t i= 0; qc->paths[i] != NULL; i++)
  {
    // char* slash = (char*)malloc((strlen("/" + 1)*sizeof(char)));
    // printf("After slash malloc\n");
    // strcpy(slash, "/");
    // printf("After strcpy\n");
    char* path;
    // printf("path: %s\n", path);
    size_t len = strlen(qc->paths[i]);
    path = malloc(strlen(qc->paths[i]) + 2);
    strcpy(path, qc->paths[i]);
    path[len] = '/';
    path[len + 1] = '\0';

    // strncpy(path, qc->paths[i], strlen(qc->paths[i]));
    // printf("After strncpy, path=%s\n", path);
    // char* pathAndFile = concat(path, slash);
    // printf("pathAndFile: %s\n", pathAndFile);
    char* pathAndFile = concat(path, fileName_);
    // printf("pathAndFile concat: %s\n\n", pathAndFile);
    if(access(pathAndFile, F_OK) != -1 )
    {
    // file exists
      // printf("%s appears at %s\n", fileName, pathAndFile);
      return(pathAndFile);
    } else
    {
      // printf("%s does not appear in %s\n", fileName, pathAndFile);
    // file doesn't exist
    }
    free(pathAndFile);
  }
  return(NULL);
}
