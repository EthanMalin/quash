#include "path.h"
#include "quashutils.h"

const char* PWD_FLAG = "PWD"; // This is confirmed for MacOS


// Given the char** main(),
// returns a malloced char* containing the current working directory
// must be free'd!
char* activeDirectory(char** env)
{
  size_t i = 0;
  char *r = NULL;
  while (env[i] != NULL && r == NULL)
  {
    printf("splitting on %s\n", env[i]);
    char** dir = split(env[i], "=", 3);
    if (strcmp(dir[0],PWD_FLAG) == 0)
    {
      char* ad = malloc(strlen(dir[1]) + 1);
      strncpy(ad, dir[1], strlen(dir[1]));
      ad[strlen(dir[1])] = '\0';
      r = ad;
    }

    printf("cleaning up\n");
    size_t j = 0;
    while(dir[j] != NULL)
    {
      printf("free[]\n");
      free(dir[j]);
      j++;
    }
    printf("free \n");
    //    free(dir);

    i++;
  }
  printf("returning\n");
  return r;
}

