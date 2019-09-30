#include <stdio.h> //printf
#include <stdlib.h> // NULL constant

#include "quashutils.h" //trimEnds, split
#include "inputblock.h"

// struct InputBlock;

/* printInputBlock
 * for debug purposes
 */
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


/* DESTRUCTORS -------------------------------------------------------- */
void freeInputBlock(struct InputBlock *ib) {
  // assumes execName, args, ib, all populated
  ib->next = NULL;
  ib->prev = NULL;
  free(ib->execName);
  if (ib->inputFile != NULL) { free(ib->inputFile); }
  if (ib->outputFile != NULL) { free(ib->outputFile); }
  for (int i = 0; i < ib->argc; i++) { free(ib->args[i]); }
  free(ib->args);
  free(ib);
}

void freeInputBlockLinkedList(struct InputBlock *first) {
  if (first != NULL) {
    while (first->next != NULL) {
      first = first->next;
      freeInputBlock(first->prev);
    }
    free(first);
  }
}
/* END DESTRUCTORS ---------------------------------------------------- */


/* parseInputBlockArgs
 * populates args with arguments, returns argc
 */
int parseInputBlockArgs(int blockLength, char **block, char **args) {
  // count the arguments, start at second index (first is executable name)
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


/* CONSTRUCTORS -------------------------------------------------------------------------- */
struct InputBlock* inputBlockFromString(char *rawBlock, int maxInputBlockLength) {
  struct InputBlock *ib;

  // parse block
  char *trimmedBlock = trimEnds(rawBlock); // returns substring
  char **block = split(trimmedBlock, " ", maxInputBlockLength / 2); // makes copy of block, splits into fields e.g. ["ls", "-a", "-t", ... ]
  if (block == NULL) { // this would happen if the block was too long/had too many spaces
    printf("ERROR: command string \"%s\" ill formed\n", trimmedBlock);
    return NULL;
  }

  // create InputBlock
  ib = malloc(sizeof(struct InputBlock));        
  ib->execName = trimEnds(block[0]); // trim *might* be unecessary, but be safe ig

  // no support for parsing these out yet
  ib->inputFile = NULL;
  ib->outputFile = NULL;    

  ib->argc = parseInputBlockArgs(maxInputBlockLength / 2, block, ib->args); // doing a little more work here
  return ib;
}


void createInputBlockLinkedList(struct InputBlock *first, char **rawBlocks, int maxPipelineLength, int maxInputBlockLength) {
    // bookkeeping pointers
    struct InputBlock *prev = NULL;
    struct InputBlock *current = first;
    for(int i = 0; i < maxPipelineLength; i++) {
      // rawBlocks padded with NULL
      if(rawBlocks[i] == NULL) {
	// we have reached the end of list "early", so break out here
	// prev->next already initialized to NULL
	break;
      }
      
      current = inputBlockFromString(rawBlocks[i], maxInputBlockLength);
      if (current == NULL) {
	freeInputBlockLinkedList(first);
	first = NULL;
	break;
      }

      // hook up the new node to the list
      if (prev != NULL) { prev->next = current; }
      current->next = NULL;
      current->prev = prev;

      // iterate
      prev = current;
    }
}
/* END CONSTRUCTORS ------------------------------------------------------------------------------------ */
