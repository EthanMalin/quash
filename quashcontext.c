#include "quashcontext.h"

struct QuashContext* contextCtor(char** _env)
{
  struct QuashContext* returnContextPtr = malloc(sizeof(struct QuashContext));
  returnContextPtr->env = _env;
  returnContextPtr->cwd = activeDirectory(_env);
  returnContextPtr->home = getInitialHome(_env);
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

bool set(struct QuashContext* qc, char* var, char* newVarValue)
{
  printf("var: %s, newVarValue: %s\n", var, newVarValue);
  for (char **vars = qc->env; *vars != 0; vars++)
  {
    char *thisVar = *vars;
    char** dir = split(thisVar, "=", 3);

    if(strcmp(dir[0], var) == 0 && strcmp(var, "HOME") == 0)
    {
      free(qc->home);
      qc->home = malloc(strlen(newVarValue) + 1);
      strcpy(qc->home, newVarValue);
      return(true);
    }
    else if (strcmp(thisVar, var) == 0 && strcmp(var, "PATH") == 0)
    {
      // printf("Confirmed paths\n");
      // free(qc->paths);
      // qc->paths = malloc(strlen(newVarValue) + 1);
      *qc->paths[0] = *newVarValue;
      // strcpy(qc->paths[0], newVarValue);
      return(true);
    }
  }
  return(false);
}

char* getInitialHome(char** env)
{
  size_t i = 0;
  while (env[i] != NULL)
  {
    char** dir = split(env[i], "=", 3);
    if (strcmp(dir[0], "HOME") == 0)
    {
      char* ad = malloc(strlen(dir[1]) + 1);
      strncpy(ad, dir[1], strlen(dir[1]));
      ad[strlen(dir[1])] = '\0';

      size_t j = 0;
      while(dir[j] != NULL)
      {
        free(dir[j]);
        j++;
      }
      free(dir);
      // printf("Returning %s\n", ad);
      return ad;
    }
    
    size_t j = 0;
    while(dir[j] != NULL)
    {
      free(dir[j]);
      j++;
    }
    free(dir);

    i++;
  }
  return (NULL);
}