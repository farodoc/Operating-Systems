#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int global = 0;

int main(int argc, char *argv[]){
    if(argc < 2){
        printf("Wrong arguments\n");
        return EXIT_FAILURE;
    }

    printf("Program name: %s\n", argv[0]);

    int local = 0;

    pid_t pid = fork();

    if (pid == 0){
        printf("Child process\n");
        global++;
        local++;
        printf("Child pid = %d, parent pid = %d\n", getpid(), getppid());
        printf("Child's local = %d, child's global = %d\n\n", local, global);
        return execl("/bin/ls", "ls", argv[1], NULL);
    }

    int status;
    wait(&status);
    
    printf("\nParent process\n");
    printf("Parent pid = %d, child pid = %d\n", getpid(), pid);
    printf("Child exit code: %d\n", WEXITSTATUS(status));
    printf("Parent's local = %d, parent's global = %d\n\n", local, global);
    return WEXITSTATUS(status);
}