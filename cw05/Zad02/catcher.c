#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <signal.h>
#include <unistd.h>

volatile int mode = -1;
volatile int requsest = 0;

void SIGUSR1_handler(int sig, siginfo_t *info, void *ucontext){
    mode = info->si_value.sival_int;
    requsest++;

    kill(info->si_pid, SIGUSR1);
}

int main(){
    printf("Catcher PID: %d\n", getpid());

    struct sigaction act;
    act.sa_sigaction = SIGUSR1_handler;
    act.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1, &act, NULL);

    while(1){
        switch(mode){
            case 1:
                for(int i = 0; i <= 100; i++){
                    printf("%d ", i);
                }
                printf("\n");
                mode = -1;
                break;

            case 2:
                printf("Request counter: %d\n", requsest);
                mode = -1;
                break;

            case 3:
                printf("Received exit signal\n");
                return 0;
                break;

            default:
                break;
        }
    }

    return 0;
}