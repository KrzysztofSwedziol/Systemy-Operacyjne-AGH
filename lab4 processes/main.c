#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char* argv[]) {
    int processes_amount = atoi(argv[1]);

    if(processes_amount <= 0){
        fprintf(stderr, "Liczba procesow potomnych musi byc wieksza niz 0.\n");
    }

    for(int i=0; i<processes_amount; i++){
        pid_t child_pid = fork();
        if(child_pid < 0){
            //error in process
            perror("fork");
            return 1;
        }
        if(child_pid == 0){
            //we are in childs process
            printf("Proces macierzysty: %d, Proces potomny: %d\n", getppid(), getpid());
            exit(0);
        }

        for(int i=0; i< processes_amount; i++){
            wait(NULL);
        }

    }
    return 0;
}
