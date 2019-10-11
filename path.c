#include "path.h"

const char* PWD_FLAG = "OLDPWD"; // This is confirmed for MacOS


// Given the char** envp from main(),
// returns a malloced char* containing the current working directory
// must be free'd!
char* initial_cwd(char** environ)
{
  size_t i = 0;
  while (environ[i] != NULL)
  {
    char** split_ = split(environ[i], "=", 3);

    if (strcmp(split_[0],PWD_FLAG) == 0)
    {
      char* split_1 = malloc(strlen(split_[1]));
      strlcpy(split_1, split_[1], strlen(split_[1]));

      size_t j = 0;
      while(split_[j] != NULL)
      {
        free(split_[j]);
        j++;
      }
      free(split_);
      return(split_1);
    }
    i++;
  }
  return (NULL);
}