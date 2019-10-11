#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h> //O_WRONLY, O_RDONLY

#include "inputblock.h"
#include "quashutils.h"

// constants ------------------------------------
const char *EXIT = "quit";
const int MAX_INPUT_LENGTH = 512; //arbitrary
const int MAX_INPUT_BLOCK_LENGTH = 256;
const int MAX_PIPELINE_LENGTH = 32;
// ----------

void test();
void quash(struct InputBlock *first);

int main(int argc, char **argv) {
  char *input = malloc(MAX_INPUT_LENGTH);
  //  test();
  //  return 0;
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
      // MIGHT NOT BE TRUE, does fgets allocate input?
    }

    // make InputBlocks linked list, array of size MAX_PIPELINE_LENGTH, padded with NULL at the end
    // TODO, require split to end the array with NULL
    inputPipeSplit = split(input, "|", MAX_PIPELINE_LENGTH);
    if (inputPipeSplit == NULL) {
      printf("Error splitting pipes\n");
      continue;
    }

    first = createInputBlockLinkedList(inputPipeSplit, MAX_PIPELINE_LENGTH, MAX_INPUT_BLOCK_LENGTH);
    if (first == NULL) {
      printf("ERROR: could not create linked list from strings.\n");
      break;
    }

    printf("made input block list:\n");
    struct InputBlock *traveler = first;
    while (traveler != NULL) {
      printInputBlock(traveler);
      traveler = traveler->next;
    }

    printf("quashing...\n");
    quash(first);
    printf("done quashing...\n");
    
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

void quash(struct InputBlock *first) {
  struct InputBlock *current = first;
  pid_t child;
  int pipeIn[2], pipeOut[2];
  int i = 0;

  if (first == NULL) {
    return;
  }

  printf("non-null list...\n");
  // set up for the first guy
  pipeIn[0] = current->inputFile != NULL ? open(current->inputFile, O_RDONLY) : -1;
  pipeIn[1] = -1;

  if (current->next != NULL) {
    pipe(pipeOut);
  } else {
    pipeOut[1] = current->outputFile != NULL ? open(current->outputFile, O_WRONLY) : -1;
    pipeOut[0] = -1;
  }
  printf("first guy setup...\n");
  while (current != NULL) {
    printf("executing %s...\n", current->execName);
    // pipeIn should have been set at the end of the last iteration, or first guy setup above
    if (current != first) {
      if (current->next != NULL) {	
	pipe(pipeOut);
      } else {
	pipeOut[1] = current->outputFile != NULL ? open(current->outputFile, O_WRONLY) : -1;
	pipeOut[0] = -1;
      }
    }

    printf("forking...\n");
    child = fork();
    
    // CHILD CODE HERE
    if (child == 0) {
      printf("in child...\n");
      if(pipeIn[0] != -1) { dup2(pipeIn[0], STDIN_FILENO); }
      if(pipeOut[1] != -1) { dup2(pipeOut[1], STDOUT_FILENO); }
      if(pipeOut[0] != -1) { close(pipeOut[0]); }
      if (execv(current->execName, current->args) == -1) {
	printf("uh-oh");
	exit(-1);
      }
      exit(0); 
    // END CHILD CODE
    } else {
      printf("spawned child: %d\n", child);
    }
    // child we just spawned has domain over these two now
    if (pipeIn[0] != -1) {
      printf("closing pipeIn[0]\n");
      close(pipeIn[0]);
    }
    if (pipeOut[1] != -1) {
      printf("closing pipeOut[1]\n");
      close(pipeOut[1]); }
    
    // pipeIn[1] should just be -1
    pipeIn[0] = pipeOut[0];
    pipeOut[0] = -1;
    current = current->next;
    
  }
  if (pipeOut[1] != -1) { close(pipeOut[1]); }
  wait(&child); // wait for the last forked child, for now always do this
}   

      // TESTING FUNCTION
void test() {
  
  // freed per iteration
  char **inputPipeSplit; // array of strings
  struct InputBlock *first;

  char t1[] = "ls -a";
  char t2[] = "cat sample.txt | sort +1 +2";
  char t3[] = "echo test.txt < test2.txt | cat | sort -2 -1 : somearg | man > output.txt";
  char t4[] = "ls";
  char *t[4] = {t1, t2, t3, t4};

  // testing split on pipes                                                                                                                                                                         
  for(int i = 0; i < 4; i++) {
    printf("\n\n#######################################\n");
    printf("creating linked list from: \"%s\"\n", t[i]);
    inputPipeSplit = split(t[i], "|", MAX_PIPELINE_LENGTH);
    
    first = createInputBlockLinkedList(inputPipeSplit, MAX_PIPELINE_LENGTH, MAX_INPUT_BLOCK_LENGTH);
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
}
