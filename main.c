#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <limits.h>
#include <errno.h>

#define MAX_ARGS 64
#define MAX_LINE 1024
#define PROMPT_SIZE 128

void print_prompt() {
    char cwd[PATH_MAX];
    char hostname[HOST_NAME_MAX];
    char *username = getlogin();

    if (getcwd(cwd, sizeof(cwd)) != NULL && gethostname(hostname, sizeof(hostname)) == 0) {
        printf("%s@%s: %s > ", username, hostname, cwd);
    } else {
        perror("Error fetching prompt information");
    }
}

void execute_command(char *args[], int background) {
    pid_t pid = fork();

    if (pid == 0) {
        // Child process
        if (execvp(args[0], args) == -1) {
            perror("execvp failed");
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        // Error forking
        perror("fork failed");
    } else {
        // Parent process
        if (!background) {
            int status;
            waitpid(pid, &status, 0);  // Wait for the foreground process to finish
        } else {
            printf("Process running in background with PID %d\n", pid);
        }
    }
}

void change_directory(char *path) {
    if (path == NULL) {
        path = getenv("HOME");  // Default to home directory
    }

    if (chdir(path) != 0) {
        perror("chdir failed");
    }
}

int main() {
    char line[MAX_LINE];
    char *args[MAX_ARGS];
    char *token;
    int background;

    while (1) {
        print_prompt();
        if (fgets(line, sizeof(line), stdin) == NULL) {
            printf("\n");
            break;  // Handle Ctrl-D for exit
        }

        line[strcspn(line, "\n")] = '\0';  // Remove trailing newline

        // Tokenize input
        int arg_count = 0;
        background = 0;
        token = strtok(line, " ");
        while (token != NULL) {
            args[arg_count++] = token;
            token = strtok(NULL, " ");
        }

        args[arg_count] = NULL;  // Null terminate the argument list

        // Check for empty command
        if (arg_count == 0) {
            continue;
        }

        // Check for background execution (last argument '&')
        if (strcmp(args[arg_count - 1], "&") == 0) {
            background = 1;
            args[arg_count - 1] = NULL;  // Remove '&' from arguments
        }

        // Handle built-in commands like 'cd'
        if (strcmp(args[0], "cd") == 0) {
            change_directory(args[1]);
        } else if (strcmp(args[0], "exit") == 0) {
            break;  // Exit the shell
        } else {
            execute_command(args, background);
        }
    }

    return 0;
}

