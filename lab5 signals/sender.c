#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

volatile sig_atomic_t acknowledged = 0;

void handle_act(int signum){
    if (signum == SIGUSR1){
        acknowledged = 1;
    }
}

int main(int argc, char *argv[]) {
    if(argc != 3){
        fprintf(stderr, "use: %s <catcher_pid> <mode>", argv[0]);
        exit(EXIT_FAILURE);
    }

    int catcher_pid = atoi(argv[1]);
    int mode = atoi(argv[2]);

    struct sigaction sa;
    sa.sa_handler = handle_act;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGUSR1, &sa, NULL);

    union sigval value;
    value.sival_int = mode;
    sigqueue(catcher_pid, SIGUSR1, value);

    while(!acknowledged){
        pause();
    }

    printf("Sender received acknowledgment for mode\n");

    return 0;
}