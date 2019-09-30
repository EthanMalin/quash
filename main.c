#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "inputblock.h"
#include "quashutils.h"

// constants ------------------------------------
const char *EXIT = "quit";
const int MAX_INPUT_LENGTH = 512; //arbitrary
const int MAX_INPUT_BLOCK_LENGTH = 256;
const int MAX_PIPELINE_LENGTH = 32;
// ----------

int main(int argc, char **argv) {
  char *input = malloc(MAX_INPUT_LENGTH);

  // freed per iteration
  char **inputPipeSplit; // array of strings
  struct InputBlock *first;
  
  while (true) {
    // replace with actual active directory
    printf("[activeDirectory]-->");
    fflush(stdout);

    // get & strip input
    fgets(input, MAX_INPUT_LENGTH, stdin);
    strtok(input, "\n");  // gets rid of trailing (first!) newline from fgets input

    // handle command -- eventually needs to be a branch?
    if (strcmp(EXIT, input) == 0) {
      printf("Goodbye.\n");
      break; // haven't allocated anything this iteration, safe to exit
    }

    // make InputBlocks linked list, array of size MAX_PIPELINE_LENGTH, padded with NULL at the end
    inputPipeSplit = split(input, "|", MAX_PIPELINE_LENGTH);
    if (inputPipeSplit == NULL) {
      printf("Error splitting pipes\n");
      continue;
    }

    // persistent pointer to the first element in the list
    first = malloc(sizeof(struct InputBlock));
    first->prev = NULL;
    first->next = NULL;

    createInputBlockLinkedList(first, inputPipeSplit, MAX_PIPELINE_LENGTH, MAX_INPUT_BLOCK_LENGTH);

    /*
     * actually run the commands here
     */
    
    // free inputPipeSplit every iteration
    for (int j = 0; j < MAX_PIPELINE_LENGTH; j++) {
      if (inputPipeSplit[j] == NULL) { break; }
      free(inputPipeSplit[j]);
    }
    free(inputPipeSplit);

    // free InputBlocks every iteration
    freeInputBlockLinkedList(first);
  }
  
  free(input);
  return 0;
}
