#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

void signalHandler(int signum){
    printf("We received %d signal and we're handling it\n", signum);
}

int main(int argc, char *argv[]) {
    if(argc != 2){
        fprintf(stderr, "Use argument from the list %s [none|ignore|handler|mask]\n", argv[0]);
    }
    if(strcmp(argv[1], "none") == 0){           //no change for reaction to signal
        printf("Signal was passed, We're keeping and executing program without changing it's reaction to the signal\n");
    }
    else if(strcmp(argv[1], "ignore") == 0){         //We are ignoring signal
        signal(SIGUSR1, SIG_IGN);
        printf("Signal SIGUSR1 was sent and we are ignoring it\n");
    }
    else if(strcmp(argv[1], "handler") == 0){        //We are handling signal
        signal(SIGUSR1, signalHandler);
        //raise(SIGUSR1);
    }
    else if(strcmp(argv[1], "mask") == 0){
        sigset_t newMask;
        sigemptyset(&newMask);
        sigaddset(&newMask, SIGUSR1);
        if (sigprocmask(SIG_BLOCK, &newMask, NULL) < 0){
            perror("Failed to mask the signal\n");
        }else{
            printf("SIGUSR1 signal was masked successfully\n");
        }
        sigset_t signalsPending;
        sigpending(&signalsPending);
        if(sigismember(&signalsPending, SIGUSR1)){
            printf("Signal SIGUSR1 is pending\n");
        }else{
            printf("Signal SIGUSR1 is not pending\n");
        }
        raise(SIGUSR1);
    }
    else{
        printf("Invalid argument was given\n");
        exit(EXIT_FAILURE);
    }
    if(strcmp(argv[1], "mask") == 0){
        sigset_t signalsPending;
        sigpending(&signalsPending);
        if(sigismember(&signalsPending, SIGUSR1)){
            printf("Signal SIGUSR1 is pending\n");
        }else{
            printf("Signal SIGUSR1 is not pending\n");
        }
    }
    return 0;
}
