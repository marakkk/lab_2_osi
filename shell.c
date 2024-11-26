#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

#define SHELL_MAX_INPUT 1024
#define SHELL_MAX_ARGS 100

void start_shell(void);
int execute_command(char *command);

void start_shell(void) {
    char input[SHELL_MAX_INPUT];
    
    printf("shell> ");
    while (fgets(input, SHELL_MAX_INPUT, stdin) != NULL) {
        input[strcspn(input, "\n")] = '\0';

        if (strcmp(input, "exit") == 0) {
            break;
        }
        
        execute_command(input);

        printf("shell> ");
    }
}

int execute_command(char *command) {
    pid_t pid;
    int return_status = 0;
    char *args[SHELL_MAX_ARGS];
    int i = 0;
    char *token;
    struct timespec start, end;

    token = strtok(command, " ");
    while (token != NULL && i < SHELL_MAX_ARGS - 1) {
        args[i] = token;
        i++;
        token = strtok(NULL, " ");
    }
    args[i] = NULL;

    clock_gettime(CLOCK_REALTIME, &start);

    pid = fork();
    if (pid == 0) {
        execvp(args[0], args);
        perror("execvp failed");
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        waitpid(pid, &return_status, 0);
        
        clock_gettime(CLOCK_REALTIME, &end);

        double elapsed = (end.tv_sec - start.tv_sec) +
                         (end.tv_nsec - start.tv_nsec) / 1e9;
        
        printf("Execution time: %.3f seconds\n", elapsed);
    } else {
        perror("fork failed");
    }
    return return_status;
}

