#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <seed> <array_size> [additional_args...]\n", argv[0]);
        printf("Example: %s 42 1000\n", argv[0]);
        return 1;
    }

    pid_t pid = fork();
    
    if (pid == -1) {
        
        perror("fork failed");
        return 1;
    } else if (pid == 0) {
        printf("Child process: PID = %d\n", getpid());
        
        char *new_argv[argc + 2]; 
        new_argv[0] = "./sequential_min_max";
        new_argv[1] = "--seed";
        new_argv[2] = argv[1];   
        new_argv[3] = "--array_size"; 
        new_argv[4] = argv[2];    
        
        for (int i = 5; i < argc + 1; i++) {
            new_argv[i] = argv[i - 3];
        }
        new_argv[argc + 1] = NULL; 
        
        printf("Launching: %s %s %s %s %s\n", 
               new_argv[0], new_argv[1], new_argv[2], 
               new_argv[3], new_argv[4]);
        
        execvp(new_argv[0], new_argv);
        
        perror("execvp failed");
        exit(1);
    } else {
        printf("Parent process: PID = %d, child PID = %d\n", getpid(), pid);
        
        int status;
        waitpid(pid, &status, 0); 
        
        if (WIFEXITED(status)) {
            printf("Child process exited with status: %d\n", WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("Child process killed by signal: %d\n", WTERMSIG(status));
        }
        
        printf("Parent process finished\n");
    }
    
    return 0;
}