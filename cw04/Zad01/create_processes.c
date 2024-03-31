#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[]){
    if(argc < 2){
        printf("Wrong arguments\n");
        return EXIT_FAILURE;
    }

    int n = atoi(argv[1]);

    for(int i = 0; i < n; i++){
        pid_t pid = fork();

        if(pid == 0){
            printf("Parent process pid: %d      Child process pid: %d\n", getppid(), getpid());
            return EXIT_SUCCESS;
        }
    }

    while(wait(NULL) > 0);

    printf("Number of processes: %d\n", n);
}