#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <pwd.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>

#define MAX_BG_JOBS 100

// structure to store information about background jobs
typedef struct {
    pid_t pid;
    char cmd[256];
} bg_job;

bg_job bg_jobs[MAX_BG_JOBS];
int bg_job_count = 0;

// prints shell prompt with username, hostname, and current directory
void print_prompt() {
    char cwd[1024];
    char hostname[1024];
    char *username = getlogin();
    
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        gethostname(hostname, sizeof(hostname));
        printf("%s@%s: %s > ", username, hostname, cwd);
    } else {
        perror("getcwd() error");
    }
}

// executes a command in foreground
void execute_foreground(char **args) {
    pid_t pid = fork();
    if (pid == 0) {
        // child process: execute the command
        if (execvp(args[0], args) == -1) {
            perror("execvp error");
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        // fork failed
        perror("fork error");
    } else {
        // parent process: wait for child to finish
        int status;
        waitpid(pid, &status, 0);
    }
}

// executes a command in background
void execute_background(char **args) {
    pid_t pid = fork();
    if (pid == 0) {
        // child process: execute command
        if (execvp(args[1], &args[1]) == -1) {
            perror("execvp error");
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        // Fork failed
        perror("fork error");
    } else {
        // parent process: add job to background job list
        if (bg_job_count < MAX_BG_JOBS) {
            bg_jobs[bg_job_count].pid = pid;
            strcpy(bg_jobs[bg_job_count].cmd, args[1]);
            bg_job_count++;
        }
        printf("[%d]: %s running in background\n", pid, args[1]);
    }
}

// changes current working directory
void change_directory(char *path) {
    if (path == NULL || strcmp(path, "~") == 0) {
        path = getenv("HOME");
    }

    if (chdir(path) != 0) {
        perror("chdir error");
    }
}

// lists running background jobs
void bglist() {
    printf("Background jobs:\n");
    for (int i = 0; i < bg_job_count; i++) {
        printf("%d: %s\n", bg_jobs[i].pid, bg_jobs[i].cmd);
    }
    printf("Total Background jobs: %d\n", bg_job_count);
}

// checks if background jobs have finished and removes them
void check_background_jobs() {
    for (int i = 0; i < bg_job_count; i++) {
        int status;
        pid_t result = waitpid(bg_jobs[i].pid, &status, WNOHANG); // non-blocking wait to check job status
        if (result == 0) {
            // job still running
            continue;
        } else if (result > 0) {
            // job has finished
            printf("%d: %s has terminated.\n", bg_jobs[i].pid, bg_jobs[i].cmd);
            // remove the job from list
            for (int j = i; j < bg_job_count - 1; j++) {
                bg_jobs[j] = bg_jobs[j + 1];
            }
            bg_job_count--;
        }
    }
}

int main() {
    char input[1024];
    char *args[64];
    
    // main loop: continuously prompt for user input, handle commands, and manage background jobs
    while (1) {
        print_prompt();
        fgets(input, sizeof(input), stdin);
        
        // remove newline from input
        input[strcspn(input, "\n")] = 0;
        
        // tokenize input
        int i = 0;
        args[i] = strtok(input, " ");
        while (args[i] != NULL) {
            i++;
            args[i] = strtok(NULL, " ");
        }

        // handle built-in commands
        if (args[0] == NULL) {
            continue; // if no command is entered, show prompt again
        } else if (strcmp(args[0], "cd") == 0) {
            change_directory(args[1]);
        } else if (strcmp(args[0], "exit") == 0) {
            break;  // exit the shell loop
        } else if (strcmp(args[0], "bg") == 0) {
            execute_background(args);
        } else if (strcmp(args[0], "bglist") == 0) {
            bglist();
        } else {
            // foreground execution
            execute_foreground(args);
        }

        check_background_jobs();
    }

    return 0;
}
