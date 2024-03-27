#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int global = 0;

int main(int argc, char *argv[]){
    printf("%s\n", argv[0]);

    int local = 0;
    pid_t child = fork();

    if(child < 0){
        perror("fork");
        exit(EXIT_FAILURE);
    }
    else if(child == 0){
        printf("child process\n");
        global++;
        local++;
        printf("child pid = %d, parent pid = %d\n", getpid(), getppid());
        printf("child's local = %d, child's global = %d\n", local, global);

        int exec_value = execl("bin/ls", "ls", argv[1], NULL);
        printf("execl error code: %d\n", exec_value);
        exit(exec_value);
    }else{
        printf("parent process: %d\n");

        int error_status;
        waitpid(child, &error_status, 0);
        printf("parent pid: %d, child pid %d\n", getpid(), child);

        if(WIFEXITED(error_status)){
            printf("Child exit code: %d\n", WEXITSTATUS(error_status));
        }
        printf("Parent's local = %d, parent's global = %d\n", local, global);

    }
    return EXIT_SUCCESS;
}