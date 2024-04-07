#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>

void SIGUSR1_handler(int sig){
    printf("Received SIGUSR1 signal: %d\n", sig);
}

int main(int argc, char *argv[]) {
    if(argc < 2){
        printf("Usage: %s <none|ignore|handler|mask>\n", argv[0]);
        return -1;
    }

    if(strcmp(argv[1], "none") == 0){
        signal(SIGUSR1, SIG_DFL);
    }
    else if(strcmp(argv[1], "ignore") == 0){
        signal(SIGUSR1, SIG_IGN);
    }
    else if(strcmp(argv[1], "handler") == 0){
        signal(SIGUSR1, SIGUSR1_handler);
    }
    else if(strcmp(argv[1], "mask") == 0){
        sigset_t mask;
        sigemptyset(&mask);
        sigaddset(&mask, SIGUSR1);
        sigprocmask(SIG_SETMASK, &mask, NULL);

        sigset_t pending;
        sigpending(&pending);
        if(sigismember(&pending, SIGUSR1)){
            printf("SIGUSR1 is pending\n");
        }
        else{
            printf("SIGUSR1 is not pending\n");
        }
    }
    else{
        signal(SIGUSR1, SIG_DFL);
    }

    raise(SIGUSR1);

    if(strcmp(argv[1], "mask") == 0){
        sigset_t pending;
        sigpending(&pending);
        if(sigismember(&pending, SIGUSR1)){
            printf("SIGUSR1 is pending\n");
        }
        else{
            printf("SIGUSR1 is not pending\n");
        }
    }
}