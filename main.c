#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

const char *EXIT = "q";

int main(int argc, char **argv) {
  bool quit = false;
  char *input = malloc(256);
  
  while (!quit) {
    // prompt
    printf("\n >");
    fflush(stdout);

    // get & strip input
    fgets(input, 256, stdin);
    strtok(input, "\n");  // gets rid of trailing newline from fgets input

    // handle command
    if (strcmp(EXIT, input) == 0) {
      printf("Goodbye.\n");
      quit = true;
    } else {
      printf("You entered: %s with length %lu", input, strlen(input));
    }

  }

  // deallocate resources
  free(input);
  return 0;
}
   
