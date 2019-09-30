#ifndef INPUT_BLOCK_H
#define INPUT_BLOCK_H

struct InputBlock {
  struct InputBlock *prev;
  struct InputBlock *next;
  char *execName;
  // WARNING -- if files unused they MUST explicitly be set to NULL
  char *inputFile; 
  char *outputFile;
  int argc;
  // C requires that this be the last element in the struct
  char **args;
};

void printInputBlock(struct InputBlock *ib);
void freeInputBlock(struct InputBlock *ib);
void createInputBlockLinkedList(struct InputBlock *first, char **raws, int maxPipelineLength, int maxInputBlockLength);
void freeInputBlockLinkedList(struct InputBlock *first);
#endif
