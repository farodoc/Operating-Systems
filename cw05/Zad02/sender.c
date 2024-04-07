#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

void SIGUSR1_handler(int sig){
    printf("Received signal %d\n", sig);
}

int main(int argc, char *argv[]){
    if(argc < 3){
        printf("Usage: %s <catcher_pid> <mode>\n", argv[0]);
        return -1;
    }

    int catcher_pid = atoi(argv[1]);
    int mode = atoi(argv[2]);

    signal(SIGUSR1, SIGUSR1_handler);

    union sigval val;
    val.sival_int = mode;
    sigqueue(catcher_pid, SIGUSR1, val);

    sigset_t mask;
    sigfillset(&mask);
    sigdelset(&mask, SIGUSR1);
    sigdelset(&mask, SIGINT);

    sigsuspend(&mask);

    return 0;
}