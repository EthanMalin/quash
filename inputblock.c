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
int parseInputBlockArgs(int blockLength, char **block, char ***args) {
  // count the arguments, start at second index (first is executable name)
  int argc = 0;
  for (;argc < blockLength; argc++) {
    if (block[argc+1] == NULL) {
      break;
    }
  }
  // populate the arguments
  *args = malloc(argc * sizeof(char *));
  for (int i = 0; i < argc; i++) {
    (*args)[i] = block[i+1];
  }

  return argc;
}


/* CONSTRUCTORS -------------------------------------------------------------------------- */
void inputBlockFromString(struct InputBlock **ib,char *rawBlock, int maxInputBlockLength) {
  printf("creating InputBlock from \"%s\"\n", rawBlock);

  // parse block
  char *trimmedBlock = trimEnds(rawBlock); // returns substring TODO, MAKE RETURN COPY
  char **block = split(trimmedBlock, " ", maxInputBlockLength / 2); // makes copy of block, splits into fields e.g. ["ls", "-a", "-t", ... ]
  
  if (block == NULL) { // this would happen if the block was too long/had too many spaces
    printf("ERROR: command string \"%s\" ill formed\n", trimmedBlock);
    (*ib) = NULL;
  }

  // create InputBlock
  (*ib) = malloc(sizeof(struct InputBlock));
  (*ib)->prev = NULL;
  (*ib)->next = NULL;      
  (*ib)->execName = block[0];


  // no support for parsing these out yet
  (*ib)->inputFile = NULL;
  (*ib)->outputFile = NULL;    

  (*ib)->argc = parseInputBlockArgs(maxInputBlockLength / 2, block, &((*ib)->args)); // doing a little more work here
}


void createInputBlockLinkedList(struct InputBlock **first, char **rawBlocks, int maxPipelineLength, int maxInputBlockLength) {
  // bookkeeping pointers
  struct InputBlock *reset = *first;
  struct InputBlock *prev = NULL;
  struct InputBlock **current = first;
  for(int i = 0; i < maxPipelineLength; i++) {
    // rawBlocks padded with NULL
    if(rawBlocks[i] == NULL) { break; }

    inputBlockFromString(current, rawBlocks[i], maxInputBlockLength);
    if (*current == NULL) {
      printf("inputBlockFromString failed on iteration %d\n", i);
      freeInputBlockLinkedList(*first);
      *first = NULL;
      break;
    }

    // hook up the new node to the list
    if (prev != NULL) { prev->next = *current; }
    (*current)->next = NULL;
    (*current)->prev = prev;

    // iterate
    prev = *current;
  }

  // reset the pointer to first -- better way to do this?
  while((*current)->prev != NULL) {
    *current = (*current)->prev;
  }
}
/* END CONSTRUCTORS ------------------------------------------------------------------------------------ */
