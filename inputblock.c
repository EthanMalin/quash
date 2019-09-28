#include <stdio.h> //printf
#include <stdlib.h> // NULL constant

#include "quashutils.h" //trimEnds, split
#include "inputblock.h"

// struct InputBlock;

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

void createInputBlockLinkedList(struct InputBlock *first, char **raws, int maxPipelineLength, int maxInputBlockLength, int maxInputLength) {
    // bookkeeping pointers
    struct InputBlock *prev = NULL;
    struct InputBlock *current = first;
    for(int i = 0; i < maxPipelineLength; i++) {
      if(raws[i] == NULL) {
	// prev->next already initialized to NULL, just break
	break;
      }
      char *trimmedBlock = trimEnds(raws[i]); // returns substring
      char **blockSpaceSplit = split(trimmedBlock, " ", maxInputBlockLength / 2, maxInputLength); // makes copy of block
      

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

      prev = current;

      for(int j = 0; j < maxPipelineLength / 2; j++) { free(blockSpaceSplit[j]); }
      free(blockSpaceSplit);
    }
}
// void freeInputBlock(struct InputBlock *ib) 
