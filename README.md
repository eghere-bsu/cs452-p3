# P3 - Mini Shell Project
### By Ethan Ghere

## Overview
This project represents a simple shell. The user can use this shell to 
run commands which work on a Linux system. 

## Compiling and Usage

To compile the program, use:
```bash
make
```

To run the program, use:
```
./myprogram
```
You can print the current version of the shell with `./myprogram -v`.

## Program Architecture

When the program is run, the user will be presented with:

```
shell>
```

The program uses the GNU Readline Library to accept input from the user. 
The user can enter commands that a traditional command line would recognize. 
There are a list of built-in commands which the program will handle 
internally. These are stored in an array which contains the name of the
command and the function associated with the command to execute. 

```
static const builtin_command builtins[] = {
    {"exit", handle_exit},
    {"cd", handle_cd},
    {"ls", handle_ls},
    {"history", handle_history},
    {"pwd", handle_pwd}
};
```

Example of usage:
```
$ ./myprogram 
shell>ls
LICENSE  Makefile  README.md  app  build  myprogram  src  test-lab  tests
shell>pwd
/workspaces/cs452-p3
shell>history
1: ls
2: pwd
shell>exit
```

If a program is not part of the built-in list, the shell will attempt to 
fork a new process and formats the arguments to work with `execvp()`. 

```
$ ./myprogram 
shell>echo hello
hello
shell>exit
```

## Note to the Grader

I had to add some `free()` functions to the test `test_cmd_parse2`, as 
this test was causing memory leaks. My implementation of the code requires 
that this is here in order to not cause memory leaks. 