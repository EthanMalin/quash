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

void freeInputBlock(struct InputBlock *ib) {
  ib->next = NULL;
  ib->prev = NULL;
  free(ib->execName);
  if (ib->inputFile != NULL) { free(ib->inputFile); }
  if (ib->outputFile != NULL) { free(ib->outputFile); }
  for (int i = 0; i < ib->argc; i++) { free(ib->args[i]); }
  free(ib->args);
  free(ib);
}

int parseArgs(int blockLength, char **block, char **args) {
  // count the arguments
  int argc = 1;
  for (;argc < blockLength; argc++) {
    if (block[argc-1] == NULL) {
      break;
    }
  }

  // populate the arguments
  args = malloc(argc * sizeof(char *));
  for (int i = 1; i < argc; i++) {
    args[i-1] = block[i];
  }

  return argc;
}

void createInputBlockLinkedList(struct InputBlock *first, char **rawBlocks, int maxPipelineLength, int maxInputBlockLength) {
    // bookkeeping pointers
    struct InputBlock *prev = NULL;
    struct InputBlock *current = first;
    for(int i = 0; i < maxPipelineLength; i++) {
      if(rawBlocks[i] == NULL) {
	// prev->next already initialized to NULL, just break
	break;
      }
      char *trimmedBlock = trimEnds(rawBlocks[i]); // returns substring
      char **block = split(trimmedBlock, " ", maxInputBlockLength / 2); // makes copy of block
      if (block == NULL) { // this would happen if the block was too long/had too many spaces
	// bail?
	current = prev;
	while (current != first) {
	  current = current->prev;
	  freeInputBlock(current->next);
	}
	freeInputBlock(first);
	first = NULL;
	break;
      }

      // allocate space and hook list up
      current = malloc(sizeof(struct InputBlock));      
      if (prev != NULL) { prev->next = current; }
      current->next = NULL;
      current->prev = prev;

      current->execName = trimEnds(block[0]); // trim might be unecessary
      current->inputFile = NULL;
      current->outputFile = NULL;
      
      current->argc = parseArgs(maxInputBlockLength / 2, block, current->args); // doing a little more work here
      
      prev = current;

      // clean up per-iteration dynamic data
      for(int j = 0; j < maxPipelineLength / 2; j++) { free(block[j]); }
      free(block);
    }
}
// void freeInputBlock(struct InputBlock *ib) 
