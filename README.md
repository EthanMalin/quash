# quash
bash like shell to practice syscalls for EECS 678 Operating Systems at KU - In collaboration with Ron Huff

The program is written in C.

BUILD
`make` in quash directory

RUN
`./quash` after building

Before executing the command, we have to parse it into a usable state. We achieve this using some custom c-string manipulation functions and parsing a command into a list of InputBlock structs where

struct InputBlock {
       struct InputBlock *prev;
       struct InputBlock *next;

       char *command;
       char *execName;

       char *inFile;
       char *outFile;

       char **args;
};

After parsing into an InputBlock list, we loop through the list, forking a child for each one and hooking up the pipes accordingly, see the `quash` and `run` methods in main.c

The program currently supports the following
[x] Running single commands as child process
[x] Giving arguments to child process
[x] Redirecting child process 'input from'/'output to' a file
[x] Linking multiple child processes together using pipes and input/output redirection
[x] Background/Foreground execution using the special character '&'
[x] Quash searches CWD and PATH variables for executables not specified with an absolute path
[x] Quash supports the build in functions `set`, `cd *dir*`, `quit`, and `exit`.
[x] Finished background processes print when done

The program currently *does not* support the following
[] The keyword `jobs` prints all running background processes
[] Child processes inherit environment variables