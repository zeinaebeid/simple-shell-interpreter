# Simple Shell Interpreter (SSI)

**CSc 360: Operating Systems (Fall 2024)**  
**Programming Assignment 1 (P1)**  
**Author**: Zeina Ebeid  
**Date**: October 4, 2024

---

## 1. Overview

This project implements a **Simple Shell Interpreter (SSI)** in C, which provides basic shell functionalities such as:
- Foreground execution of commands.
- Changing directories with the `cd` command.
- Background execution of commands using `&`.
- Listing and managing background processes with `bglist`.

The SSI displays a prompt in the format:
```
username@hostname: /current/directory >
```

The shell accepts user input, executes commands, and manages background processes, continuing to accept input while background jobs run.

---
## 2. Features

### 2.1 Foreground Execution
- The SSI allows the user to execute any valid command in the foreground.
- If the command is invalid, the shell prints an appropriate error message.


### 2.2 Directory Management (`cd`)
- The built-in `cd` command allows the user to change the current working directory.
- Supports relative and absolute paths, as well as special cases like `cd ~` to go to the home directory and `cd` without arguments to return to the home directory.


### 2.3 Background Execution (`bg`)
- Users can run commands in the background using the `bg` command.
- The shell will continue to accept input without waiting for the background process to finish.

### 2.4 Background Jobs Management (`bglist`)
- The `bglist` command prints a list of all background jobs, showing their process IDs and the associated commands.
- When a background job finishes, the SSI will notify the user.

---

## 3. Compilation and Usage

### 3.1 Compilation
- Use the provided `Makefile` to compile the program.
- Run the following command to compile the SSI: 'make' 
- This will generate an executable named `ssi`.

### 3.2 Running the Shell
- To run the shell, execute the compiled program, 
- The shell will display a prompt where users can enter commands.

### 3.3 Exiting the Shell
- Use the `exit` command to terminate the shell:

----
## 4. Known Issues and Limitations

- The shell does not support complex piping (`|`) or redirection (`>`, `<`) as in more advanced shells.
- Background jobs are limited to 100 by design (`MAX_BG_JOBS`).
- The `cd` command only supports one argument and does not handle multiple arguments.

---

## 5. References

- [Linux Manual Pages](https://man7.org/linux/man-pages/) for system calls like `fork()`, `execvp()`, and `waitpid()`.
- [UVic's CSC 360 Course Material](https://www.uvic.ca/).
