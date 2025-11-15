#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: %s <seed> <array_size>\n", argv[0]);
        printf("Example: %s 42 1000000\n", argv[0]);
        return 1;
    }

    pid_t pid = fork();
    
    if (pid == -1) {
        perror("fork failed");
        return 1;
    } else if (pid == 0) {
        printf("Child process: PID = %d\n", getpid());
        
        char *args[] = {
            "./sequential_min_max",  // имя программы
            argv[1],                 // seed (первый аргумент)
            argv[2],                 // array_size (второй аргумент)
            NULL                     // завершаем NULL
        };
        
        printf("Launching: %s %s %s\n", args[0], args[1], args[2]);
        
        execvp(args[0], args);
        
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