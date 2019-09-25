#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
  char *name = malloc(256);
  printf("What is your name?\n");
  fgets(name, 256, stdin);
  printf("Nice to meet you %s\n", name);
  free(name);
  return 0;
}
   
