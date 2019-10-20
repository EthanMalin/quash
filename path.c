#include "path.h"
#include "quashutils.h"

const char* PWD_FLAG = "PWD"; // This is confirmed for MacOS


// Given the char** main(),
// returns a malloced char* containing the current working directory
// must be free'd!
char* activeDirectory(char** env)
{
  size_t i = 0;
  while (env[i] != NULL)
  {
    char** dir = split(env[i], "=", 3);
    if (strcmp(dir[0],PWD_FLAG) == 0)
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
      printf("Returning %s\n", ad);
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

void printAllEnvVars(char** env)
{
  for (char **vars = env; *vars != 0; vars++)
  {
    char *thisVar = *vars;
    printf("%s\n", thisVar);    
  }
}
