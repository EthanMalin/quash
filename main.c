#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

const char *EXIT = "q";
const int MAX_INPUT_LENGTH = 512; //arbitrary for now
const int MAX_PIPELINE_LENGTH = 32; // also arbitrary
const int MAX_INPUT_BLOCK_LENGTH = 256; // ... you guessed it

struct InputBlock {
  char *execName;
  // WARNING -- if files unused they MUST explicitly be set to NULL
  char *inputFile; 
  char *outputFile;
  int argc;
  // C requires that this be the last element in the struct
  char *args[];
};

// for debug purposes
void printInputBlock(struct InputBlock *ib) {
  printf("Name: %s\n", ib->execName);
  printf("Args:\n");
  for (int i = 0; i < ib->argc; i++) {
    printf("\t%s\n", ib->args[i]);
  }
  if (ib->inputFile != NULL) {
    printf("Input Redirect: %s\n", ib->inputFile);
  }
  if (ib->outputFile != NULL) {
    printf("Output Redirect: %s\n", ib->outputFile);
  }
}
  
char** split(char *input, char *delim) {
  // result is an array of MAX_PIPELINE_LENGTH pointers
  char **res;
  res = malloc(MAX_PIPELINE_LENGTH * sizeof(char*));
  
  for(int i = 0; i < MAX_PIPELINE_LENGTH; i++) { res[i] = NULL; } // initialize to NULL, necessary?
  char *ptr = strtok(input, delim); // feed strtok and get first ptr
  int i = 0;
  while (ptr != NULL) {
    // if we have gotten too many inputs, free and abort, returning NULL
    if (i > MAX_PIPELINE_LENGTH - 1) {
      printf("Pipeline too long! Aborting...\n");
      for (int i = 0; i < MAX_PIPELINE_LENGTH; i++) { free(res[i]); }
      free(res);
      return NULL;
    }
    // append to res
    printf("Adding to res: %s\n", ptr);
    res[i] = malloc(MAX_INPUT_BLOCK_LENGTH);
    strcpy(res[i], ptr);
    i++;
    // get next string from strtok
    ptr = strtok(NULL, delim);
  }
  return res;
}
 
int main(int argc, char **argv) {
  bool quit = false;
  char *input = malloc(MAX_INPUT_LENGTH);
  char **inputPipeSplit;
  while (!quit) {
    // prompt
    printf("[activeDirectory]-->");
    fflush(stdout);

    // get & strip input
    fgets(input, MAX_INPUT_LENGTH, stdin);
    strtok(input, "\n");  // gets rid of trailing newline from fgets input

    inputPipeSplit = split(input, "|");
    for(int i = 0; i < MAX_PIPELINE_LENGTH; i++) {
      if (inputPipeSplit[i] == NULL) {
	break;
      } else {
	printf("block %d: %s\n", i, inputPipeSplit[i]);
      }
    }
    
    // handle command
    if (strcmp(EXIT, input) == 0) {
      printf("Goodbye.\n");
      quit = true;
    } else {
      printf("You entered: %s with length %lu\n", input, strlen(input));
    }

  }

  // deallocate resources
  free(input);
  return 0;
}
   
