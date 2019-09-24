# quash
bash like shell to practice syscalls for EECS 678 Operating Systems at KU - In collaboration with Ron Huff

- [] quash should be able to run executables with command line parameters. (fork and exec)
- [] If the executable is not specified as absolute path ('/'), quash should search in directories in the PATH environment variable, print error message if no executable found
+ [] quash should allow foreground and background ('&') executions. (wait, waitpid, SIGCHLD)
+ [] quash should support the following built-in functions:
  - [] *set* to set the value of an environment variable. Initial environment variables should be inherited from the command line the invokes quash. At least PATH and HOME must be supported on initialization. Child processes should inherit environment variables.
  - [] *cd **dir*** to change the current working directory to **dir**. *cd* with no arguments should change to the HOME directory. (chdir)
  - [] *quit* and *exit* to exit quash.
  - *jobs* should print all of the currently running background processes in the format [JOBID] PID COMMAND where JOBID is a unique positive integer quash assignes to the job to identify it, PID
+ [] When a command is run in the background, print: [JOBID] PID running in background
+ [] When a background command finishes, quash should print: [JOBID] PID finished COMMAND
+ [] quash should implement I/O redirection. ('<' - stdout from a file) ('>' - stdout to a file) ex. 'ls > a' sends results of ls to file a (freopen)
+ [] quash should implement the pipe command ('|')
+ [] quash should support reading commands interactively (with a prompt) or reading a set of commands stored in a file using '<' ex. bash> quash < commands.txt
