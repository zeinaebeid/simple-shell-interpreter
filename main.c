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

typedef struct {
    pid_t pid;
    char cmd[256];
} bg_job;

bg_job bg_jobs[MAX_BG_JOBS];
int bg_job_count = 0;

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

void execute_foreground(char **args) {
    pid_t pid = fork();
    if (pid == 0) {
        // Child process: execute the command
        if (execvp(args[0], args) == -1) {
            perror("execvp error");
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        // Fork failed
        perror("fork error");
    } else {
        // Parent process: wait for the child to finish
        int status;
        waitpid(pid, &status, 0);
    }
}

void execute_background(char **args) {
    pid_t pid = fork();
    if (pid == 0) {
        // Child process: execute the command
        if (execvp(args[1], &args[1]) == -1) {
            perror("execvp error");
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        // Fork failed
        perror("fork error");
    } else {
        // Parent process: add job to the background job list
        if (bg_job_count < MAX_BG_JOBS) {
            bg_jobs[bg_job_count].pid = pid;
            strcpy(bg_jobs[bg_job_count].cmd, args[1]);
            bg_job_count++;
        }
        printf("[%d]: %s running in background\n", pid, args[1]);
    }
}

void change_directory(char *path) {
    if (path == NULL || strcmp(path, "~") == 0) {
        path = getenv("HOME");
    }

    if (chdir(path) != 0) {
        perror("chdir error");
    }
}

void bglist() {
    printf("Background jobs:\n");
    for (int i = 0; i < bg_job_count; i++) {
        printf("%d: %s\n", bg_jobs[i].pid, bg_jobs[i].cmd);
    }
    printf("Total Background jobs: %d\n", bg_job_count);
}

void check_background_jobs() {
    for (int i = 0; i < bg_job_count; i++) {
        int status;
        pid_t result = waitpid(bg_jobs[i].pid, &status, WNOHANG);
        if (result == 0) {
            // Job is still running
            continue;
        } else if (result > 0) {
            // Job has finished
            printf("%d: %s has terminated.\n", bg_jobs[i].pid, bg_jobs[i].cmd);
            // Remove the job from the list
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
    
    while (1) {
        print_prompt();
        fgets(input, sizeof(input), stdin);
        
        // Remove newline from input
        input[strcspn(input, "\n")] = 0;
        
        // Tokenize the input
        int i = 0;
        args[i] = strtok(input, " ");
        while (args[i] != NULL) {
            i++;
            args[i] = strtok(NULL, " ");
        }

        // Handle built-in commands
        if (args[0] == NULL) {
            continue;
        } else if (strcmp(args[0], "cd") == 0) {
            change_directory(args[1]);
        } else if (strcmp(args[0], "exit") == 0) {
            break;
        } else if (strcmp(args[0], "bg") == 0) {
            execute_background(args);
        } else if (strcmp(args[0], "bglist") == 0) {
            bglist();
        } else {
            // Foreground execution
            execute_foreground(args);
        }

        check_background_jobs();
    }

    return 0;
}
