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

  char t1[] = "ls -a";
  char t2[] = "cat sample.txt | sort +1 +2";
  char t3[] = "echo test.txt | cat | sort -2 -1 : somearg | man";
  char t4[] = "ls";
  char *t[4] = {t1, t2, t3, t4};

  // testing split on pipes                                                                                                                                                                         
  for(int i = 0; i < 4; i++) {
    printf("creating linked list from: \"%s\"\n", t[i]);
    inputPipeSplit = split(t[i], "|", MAX_PIPELINE_LENGTH);
    
    createInputBlockLinkedList(&first, inputPipeSplit, MAX_PIPELINE_LENGTH, MAX_INPUT_BLOCK_LENGTH);
    if (first == NULL) {
      printf("ERROR: could not create linked list from strings.\n");
      break;
    }

    while (first != NULL) {
      printInputBlock(first);
      first = first->next;
    }

    for (int j = 0; j < MAX_PIPELINE_LENGTH; j++) {
      if (inputPipeSplit[j] == NULL) { break; }
      free(inputPipeSplit[j]);
    }
    free(inputPipeSplit);

    freeInputBlockLinkedList(first);
  }
  
  // while (true) {
  //   // replace with actual active directory
  //   printf("[activeDirectory]-->");
  //   fflush(stdout);

  //   // get & strip input
  //   fgets(input, MAX_INPUT_LENGTH, stdin);
  //   strtok(input, "\n");  // gets rid of trailing (first!) newline from fgets input

  //   // handle command -- eventually needs to be a branch?
  //   if (strcmp(EXIT, input) == 0) {
  //     printf("Goodbye.\n");
  //     break; // haven't allocated anything this iteration, safe to exit
  //   }

  //   // make InputBlocks linked list, array of size MAX_PIPELINE_LENGTH, padded with NULL at the end
  //   inputPipeSplit = split(input, "|", MAX_PIPELINE_LENGTH);
  //   if (inputPipeSplit == NULL) {
  //     printf("Error splitting pipes\n");
  //     continue;
  //   }

  //   // persistent pointer to the first element in the list
  //   first = malloc(sizeof(struct InputBlock));
  //   first->prev = NULL;
  //   first->next = NULL;

  //   createInputBlockLinkedList(first, inputPipeSplit, MAX_PIPELINE_LENGTH, MAX_INPUT_BLOCK_LENGTH);

  //   /*
  //    * actually run the commands here
  //    */
    
  //   // free inputPipeSplit every iteration
  //   for (int j = 0; j < MAX_PIPELINE_LENGTH; j++) {
  //     if (inputPipeSplit[j] == NULL) { break; }
  //     free(inputPipeSplit[j]);
  //   }
  //   free(inputPipeSplit);

  //   // free InputBlocks every iteration
  //   freeInputBlockLinkedList(first);
  // }
  
  free(input);
  return 0;
}
