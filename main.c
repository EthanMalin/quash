#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

const char *EXIT = "q";
const int MAX_INPUT_LENGTH = 512; //arbitrary for now
const int MAX_PIPELINE_LENGTH = 32; // also arbitrary
const int MAX_INPUT_BLOCK_LENGTH = 256; // ... you guessed it

struct InputBlock {
  struct InputBlock *prev;
  struct InputBlock *next;
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
  
  for(int i = 0; i < MAX_PIPELINE_LENGTH; i++) { res[i] = NULL; } // initialize to NULL
  char *ptr = strtok(input, delim); // feed strtok and get first ptr
  int i = 0;
  while (ptr != NULL) {
    // if we have gotten too many inputs, free and abort, returning NULL
    if (i == MAX_PIPELINE_LENGTH) {
      printf("Pipeline too long! Aborting...\n");
      for (int i = 0; i < MAX_PIPELINE_LENGTH; i++) { free(res[i]); }
      free(res);
      return NULL;
    }
    // append to res
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
    strtok(input, "\n");  // gets rid of trailing (first!) newline from fgets input

    // handle command
    if (strcmp(EXIT, input) == 0) {
      printf("Goodbye.\n");
      quit = true;
    } else {
      printf("You entered: %s with length %lu\n", input, strlen(input));
    }

    // make InputBlocks linked list
    // split and debug output input
    inputPipeSplit = split(input, "|");
    for(int i = 0; i < MAX_PIPELINE_LENGTH; i++) {
      if (inputPipeSplit[i] == NULL) {
	break;
      } else {
	printf("block %d: %s\n", i, inputPipeSplit[i]);
      }
    }

    // persistent pointer to the first element in the list
    struct InputBlock *first = malloc(sizeof(struct InputBlock));
    first->prev = NULL;
    first->next = NULL;

    // pointers for initializing the list (bookkeeping pointers)
    struct InputBlock *prev = NULL;
    struct InputBlock *current = first;
    for(int i = 0; i < MAX_PIPELINE_LENGTH; i++) {
      if(inputPipeSplit[i] == NULL) {
	// prev->next already initialized to NULL, just break
	break;
      }

      // allocate space and hook list up
      current = malloc(sizeof(struct InputBlock));      
      if (prev != NULL) { prev->next = current; }
      current->next = NULL;
      current->prev = prev;
      
      // TODO need functions to break inputPipeSplit[i] into string fields 
      // something like..
      /*
	current->execName = parseInputBlockExecName(inputPipeSplit[i]);
	current->inputFile = parseInputBlockInputFile(inputPipeSplit[i]);
	current->outputFile = parseInputBlockOutputFile(inputPipeSplit[i]);
	current->argc = parseInputBlockArgs(inputPipeSplit[i], current->args);
      */

      // set up for iteration
      prev = current;

    }

    // free InputBlocks -- should declare and free these outside while loop, but careful of unattached blocks after loop iteration!!
    struct InputBlock *eraser = first;
    while (eraser->next != NULL) {
      eraser = eraser->next;
      free(eraser->prev);
    }
    free(eraser);
    
  }

  // deallocate resources
  free(input);
  return 0;
}
   
