#include <stdio.h>  // printf
#include <stdlib.h> // NULL constant
#include <string.h> // strcpy

#include "quashutils.h" //trimEnds, split
#include "inputblock.h"

/* printInputBlock
 * for debug purposes
 */
void printInputBlock(struct InputBlock *ib) {
  printf("Name: %s\n", ib->execName);
  printf("Args: [");
  for (int i = 0; i < ib->argc; i++) {
    printf("%s", ib->args[i]);
    if (i != ib->argc - 1) { printf(","); }
  }
  printf("]\n");
  if (ib->inputFile != NULL) {
    printf("InFile: %s\n", ib->inputFile);
  }
  if (ib->outputFile != NULL) {
    printf("OutFile: %s\n", ib->outputFile);
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

char* parseInputBlockInputFile(char **block) {
  int i = 0;
  while (block[i] != NULL) {
    if (strcmp(block[i], "<")==0) {
      if (block[i+1] == NULL) {
        return NULL;
      } else {
        return trimEndsCopy(block[i+1]);
      }
    }
    i++;
  }
  return NULL;
}

char* parseInputBlockOutputFile(char **block) {
  int i = 0;
  while (block[i] != NULL) {
    if (strcmp(block[i], ">")==0) {
      if (block[i+1] == NULL) {
        return NULL;
      } else {
        return trimEndsCopy(block[i+1]);
      }
    }
    i++;
  }
  return NULL;
}
/* countInputBlockArgs
 */
int countInputBlockArgs(char **block) {
  // count the arguments, start at second index (first is executable name)
  int argc = 0;
  while (block[argc+1] != NULL) {
    if (strcmp("<", block[argc+1]) == 0) { break; }
    if (strcmp(">", block[argc+1]) == 0) { break; } // assuming that these will always be good... no extra spaces or anything
    argc++;
  }
  return argc;
}


/* CONSTRUCTORS -------------------------------------------------------------------------- */
struct InputBlock* inputBlockFromString(char *rawBlock, int maxInputBlockLength) {
  struct InputBlock* ib = NULL;
  // parse block
  char *trimmedBlock = trimEndsCopy(rawBlock);
  char **block = split(trimmedBlock, " ", maxInputBlockLength); // makes copy of block, splits into fields e.g. ["ls", "-a", "-t", ... ]
  free(trimmedBlock);
  if (block == NULL) { // this would happen if the block was too long/had too many spaces
    printf("ERROR: command string \"%s\" too long\n", rawBlock);
    return NULL;
  }

  // create InputBlock
  ib = malloc(sizeof(struct InputBlock));
  ib->prev = NULL;
  ib->next = NULL;

  // copy execName
  // should any of the following be in some sort of error handling scheme? probably.
  ib->execName = malloc(strlen(block[0]));
  strcpy(ib->execName, block[0]);

  ib->inputFile = parseInputBlockInputFile(block);
  ib->outputFile = parseInputBlockOutputFile(block);

  ib->argc = countInputBlockArgs(block);
  ib->args = malloc(ib->argc * sizeof(char *) + 1);
  int i = 0;
  for(;i < ib->argc; i++) {
    // deep copy the arguments
    ib->args[i] = malloc(strlen(block[i+1]));
    strcpy(ib->args[i], block[i+1]);
  }
  ib->args[i] = NULL; // so we can pass this directly to execve

  /* NOTE
   * ib now owns *copies* of the string data passed in the rawBlock parameter, and copied into the block array in the split call
   * so we can safely free the block data, and free the rawBlock data in main after this point.
   */
  // split pads block with NULL
  for (int i = 0; i < maxInputBlockLength; i++) {
    if (block[i] == NULL) { break; }
    free(block[i]);
  }
  free(block);
  return ib;
}


struct InputBlock* createInputBlockLinkedList(char **rawBlocks, int maxListLength, int maxInputBlockLength) {
  // bookkeeping pointers
  struct InputBlock *first = NULL;
  struct InputBlock *prev = NULL;
  struct InputBlock *current = NULL;
  // would be nice to get rid of maxListLength param, and just alwasy have a NULL at end of rawBlocks
  for(int i = 0; i < maxListLength; i++) {
    // rawBlocks padded with NULL
    if(rawBlocks[i] == NULL) { break; }
    current = inputBlockFromString(rawBlocks[i], maxInputBlockLength);
    if (current == NULL) {
      printf("inputBlockFromString failed on iteration %d\n", i);
      freeInputBlockLinkedList(first);
      return NULL;
    }
    if (i == 0) { first = current; }

    // hook up the new node to the list
    if (prev != NULL) { prev->next = current; }
    current->next = NULL;
    current->prev = prev;

    // iterate
    prev = current;
    current = current->next;
  }
  return first;
}
/* END CONSTRUCTORS ------------------------------------------------------------------------------------ */
