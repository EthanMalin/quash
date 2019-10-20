#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h> //O_WRONLY, O_RDONLY

#include "inputblock.h"
#include "quashutils.h"

// constants ------------------------------------
const char *EXIT = "exit";
const char *QUIT = "quiv";
const int MAX_INPUT_LENGTH = 512; //arbitrary
const int MAX_INPUT_BLOCK_LENGTH = 256;
const int MAX_PIPELINE_LENGTH = 32;
// ----------

void test();
void quash(struct InputBlock *first, bool background);
int run(struct InputBlock *toRun, int in, int out[2], pid_t *child); // helper function for quash

int main(int argc, char **argv) {
  char *input = malloc(MAX_INPUT_LENGTH);
  //  test();
  //  return 0;
  // freed per iteration
  char **inputPipeSplit; // array of strings
  struct InputBlock *first;
  bool bg; //background
  while (true) {
    // replace with actual active directory
    printf("[activeDirectory]-->");
    fflush(stdout);

    // get & strip input, definitely need more error handling here
    fgets(input, MAX_INPUT_LENGTH, stdin);
    strtok(input, "\n");  // gets rid of trailing (first!) newline from fgets input

    // handle command -- eventually needs to be a branch?
    if (strcmp(EXIT, input) == 0 || strcmp(QUIT, input) == 0) {
      printf("Goodbye.\n");
      break; // haven't allocated anything this iteration, safe to exit
      // MIGHT NOT BE TRUE, does fgets allocate input?
    }

    // look for and remove '&'
    for (int i = 0; i < MAX_INPUT_LENGTH; i++) {
      if (input[i] == '&') {
	input[i] = '\0';
	bg = true;
      } else {
	bg = false;
      }
    }

    // make InputBlocks linked list
    // make array of size MAX_PIPELINE_LENGTH, padded with NULL at the end
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

    // important
    quash(first);
    
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

void quash(struct InputBlock *first, bool background) {
  struct InputBlock *current = first;
  pid_t child;
  int out[2], in; // note 'in' NOT an array

  // set up input for first ib
  in = current->inputFile != NULL ? open(current->inputFile, O_RDONLY) : -1;

  // main execution loop
  while (current != NULL) {
    in = run(current, in, out, &child); // this function closes in and returns the next in, also populates child
    current = current->next; // iterate
  }
  if (!background) {
      wait(&child); // wait for the last forked child, for now always do this
  }
}


int run(struct InputBlock *toRun, int in, int out[2], pid_t *child) {
  // assume input is set up, set up output
  if (toRun->next != NULL) { // if there is to be a "next process", it takes precedence over output redirect
    pipe(out);
  } else if (toRun->outputFile != NULL) {
    out[1] = open(toRun->outputFile, O_WRONLY); // should test for file existence
    out[0] = -1;
  } else {
    out[1] = -1;
    out[0] = -1;
  }

  // create child and execute new process
  *child = fork();  
  if (*child == 0) {
    // CHILD CODE HERE ----------------------------------------------------------------------
    if (in != -1) { dup2(in, STDIN_FILENO); }
    if (out[1] != -1) { dup2(out[1], STDOUT_FILENO); }    
    if (out[0] != -1) { close(out[0]); } // don't need to read from output

    if (execv(toRun->execName, toRun->args) == -1) {
      printf("exec failed. aborting child (block name \"%s\")\n", toRun->execName);
      exit(-1);
    }
    // END CHILD CODE -----------------------------------------------------------------------
  } else if (*child < 0) {
    printf("error on fork: %d\n", *child);
    return -1;
  }

  if (out[1] != -1) { close(out[1]); } // no longer need write end of output in parent
  if (in != -1) { close(in); }
  return out[0]; // return potential next process input (only non-negative if we called pipe in this function)
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
