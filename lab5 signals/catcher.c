#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

volatile sig_atomic_t mode = 0;
volatile sig_atomic_t  changes = 0;

void handler(int signum, siginfo_t *siginfo, void *context){
    (void)context;                                                  //this has to be in handler for sigaction to work
    printf("I received SIGUSR1 signal\n");
    if(signum == SIGUSR1){
        mode = siginfo->si_value.sival_int;
        changes++;
        kill(siginfo->si_pid, SIGUSR1);
        if(mode == 1){
            for(int i=1; i<= 100; i++){
                printf("%d\n", i);
            }
        }else if (mode == 2){
            printf("Change requests received: %d\n", changes);
        }else if(mode == 3){
            printf("Exiting catcher program\n");
            exit(EXIT_SUCCESS);
        }
    }
}

int main(int argc, char *argv[]) {
    printf("I am cather, my pid is %d\n", getpid());

    struct sigaction act;
    act.sa_flags = SA_SIGINFO;
    act.sa_sigaction = handler;
    sigemptyset(&act.sa_mask);
    sigaction(SIGUSR1, &act, NULL);

    while(1){
        pause();
    }

    return 0;
}