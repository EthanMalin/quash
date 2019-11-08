#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h> //O_WRONLY, O_RDONLY
#include <sys/wait.h>
#include <errno.h>

#include "inputblock.h"
#include "quashutils.h"
#include "quashcontext.h"

// constants ------------------------------------
const char *EXIT = "exit";
const char *QUIT = "quit";
const int MAX_INPUT_LENGTH = 512; //arbitrary
const int MAX_INPUT_BLOCK_LENGTH = 256;
const int MAX_PIPELINE_LENGTH = 32;
// ----------

int QUASH_GLOBAL_ID_POOL = 1;
struct job {
  pid_t id;
  int qid;
  char *command;
  struct job *next;
};

void test();
void checkJobs(struct job * jobs);
void quash(struct InputBlock *first, bool background, struct QuashContext *qc);
int run(struct InputBlock *toRun, int in, int out[2], pid_t *child, struct QuashContext *qc); // helper function for quash

int main(int argc, char **argv, char **envp) {
  printf("first\n");
  char *input = malloc(MAX_INPUT_LENGTH);
  printf("second\n");
  struct QuashContext *qc = contextCtor(envp);
  printf("third\n");
  char **inputPipeSplit;
  struct InputBlock *first;
  bool bg; //background
  struct job *jobs = NULL;
  printf("fourth\n");
  while (true) {
    // check for completed processes
    pid_t exited = 0;
    int status = 0;
    do {
      exited = waitpid(0, &status, WNOHANG);
      if (exited != 0 && exited != -1) {
	printf("[%d] completed\n", exited);
      }
    } while (exited != -1 && exited != 0);

    printf("checked for completed jobs\n");
    // prompt
    printf("[%s]-->", qc->cwd);
    fflush(stdout);

    // get & strip input, definitely need more error handling here
    fgets(input, MAX_INPUT_LENGTH, stdin);
    strtok(input, "\n");  // gets rid of trailing (first!) newline from fgets input

    printf("got input\n");
    // handle exit command
    if (strcmp(EXIT, input) == 0 || strcmp(QUIT, input) == 0) {
      printf("Goodbye.\n");
      break; // haven't allocated anything this iteration, safe to exit
      // MIGHT NOT BE TRUE, does fgets allocate input?
    }

    // look for and remove '&'
    for (int i = 0; i < MAX_INPUT_LENGTH; i++) {
      if (input[i] == '&') {
	input[i] = '\0';
	bg = 1;
        printf("bg = true\n");
        break;
      } else {
	bg = 0;
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
    quash(first, bg, qc);
    
    // cleanup
    for (int j = 0; j < MAX_PIPELINE_LENGTH; j++) {
      if (inputPipeSplit[j] == NULL) { break; }
      free(inputPipeSplit[j]);
    }
    free(inputPipeSplit);
    freeInputBlockLinkedList(first);
  }
  
  contextDtor(qc);
  free(input);
  return 0;
}

void quash(struct InputBlock *first, bool background, struct QuashContext *qc) {
  struct InputBlock *current = first;
  pid_t child;
  int out[2], in; // note 'in' NOT an array

  // if the user wants to cd..
  if (strcmp(first->execName, "cd") == 0) {
    int res = chdir(first->args[1]);
    if(res < 0) {
      printf("Error on cd.\n");
      exit(-1);
    } else {
      char* slash = malloc(2);
      slash[0] = '/';
      slash[1] = '\0';
	
      char *old = qc->cwd;
      qc->cwd = concat(qc->cwd, slash);
      free(old);

      if (strcmp(first->args[1], "..") == 0) {
	deleteEnd(qc->cwd);
	size_t i = 0;
      } else {
	char *old = qc->cwd;
	qc->cwd = concat(qc->cwd, first->args[1]);
	free(old);
      }
    }
  } else {
    // set up input for first ib
    in = current->inputFile != NULL ? open(current->inputFile, O_RDONLY) : -1;

    // main executon loop
    while (current != NULL) {
      in = run(current, in, out, &child, qc); // this function closes in and returns the next in, also populates child
      current = current->next; // iterate
    }

    // handle background or not
    if (background == 0) {
      int status = 0;
      waitpid(child, &status, 0);
    }
  }
}

int run(struct InputBlock *toRun, int in, int out[2], pid_t *child, struct QuashContext *qc) {
  // assume input is set up, set up output
  if (toRun->next != NULL) { // if there is to be a "next process", it takes precedence over output redirect
    pipe(out);
  } else if (toRun->outputFile != NULL) {
    char *outFile = malloc(strlen(toRun->outputFile) + strlen(qc->cwd) + 2);
    strcpy(outFile, qc->cwd);
    strcat(outFile, "/");
    strcat(outFile, toRun->outputFile);
    outFile[strlen(toRun->outputFile) + strlen(qc->cwd) + 1] = '\0';
    out[1] = open(outFile, O_CREAT | O_WRONLY | O_EXCL, S_IRUSR | S_IWUSR); // should test for file existence
    if (out[1] < 0) {
      if (errno == EEXIST) {
	out[1] = open(outFile, O_WRONLY);
      } else {
	printf("could not open file %s\n", outFile);
      }
    }
    out[0] = -1;
  } else {
    out[1] = -1;
    out[0] = -1;
  }

  // create child and execute new process
  *child = fork();  
  if (*child == 0) {
    // CHILD CODE HERE ----------------------------------------------------------------------
    // set up input/output redirect
    if (in != -1) { dup2(in, STDIN_FILENO); }
    if (out[1] != -1) { dup2(out[1], STDOUT_FILENO); }    
    if (out[0] != -1) { close(out[0]); } // don't need to read from output

    // get full path for executable
    char* path = getFilePath(qc, toRun->execName);
    if(path == NULL) { //does not exist!!!
      printf("ERROR: Executable \"%s\" does not exist in any valid paths\n", toRun->execName);
      exit(-1);
    }

    // execute!
    if (execv(path, toRun->args) == -1) {
      printf("exec failed. aborting child (block name \"%s\")\n", toRun->execName);
      exit(-1);
    }
    // END CHILD CODE -----------------------------------------------------------------------
  } else if (*child < 0) {
    printf("error on fork: %d\n", *child);
    return -1;
  }

  // Assume successful command execution

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
