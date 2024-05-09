#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <wait.h>

#define MAX_TASKS 10
#define TASK_SIZE 11

struct task_queue {
    char tasks[MAX_TASKS][TASK_SIZE];
    int front;
    int rear;
    int count;
};

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
    struct seminfo *__buf;
};

void generate_random_letters(char letters[TASK_SIZE]) {
    srand(time(NULL) ^ getpid());

    for (int i = 0; i < TASK_SIZE - 1; i++) {
        letters[i] = 'a' + rand() % 26;
    }
    letters[TASK_SIZE - 1] = '\0';
}

int wait_semaphore(int sem_id, int sem_num) {
    struct sembuf sops;
    sops.sem_num = sem_num;
    sops.sem_op = -1;
    sops.sem_flg = 0;
    return semop(sem_id, &sops, 1);
}

int signal_semaphore(int sem_id, int sem_num) {
    struct sembuf sops;
    sops.sem_num = sem_num;
    sops.sem_op = 1;
    sops.sem_flg = 0;
    return semop(sem_id, &sops, 1);
}

int main() {
    int numberOfPrinters = 5;
    int numberOfUsers = 10;
    key_t key = ftok("bin/ls", 1);
    int shm_id = shmget(key, sizeof(struct task_queue), 0666 | IPC_CREAT);
    int sem_id = semget(key, 2, 0666 | IPC_CREAT);

    if (shm_id == -1 || sem_id == -1) {
        perror("Initialization failed");
        exit(1);
    }

    struct task_queue *queue = (struct task_queue *)shmat(shm_id, NULL, 0);
    if (queue == (void *)-1) {
        perror("shmat");
        exit(1);
    }


    union semun arg;
    arg.val = 1;
    semctl(sem_id, 0, SETVAL, arg);

    arg.val = 0;
    semctl(sem_id, 1, SETVAL, arg);

    pid_t pid;
    for (int i = 0; i < numberOfUsers + numberOfPrinters; i++) {
        pid = fork();
        if (pid == 0) {
            if (i < numberOfUsers) {

                while (1) {
                    char task[TASK_SIZE];
                    generate_random_letters(task);
                    wait_semaphore(sem_id, 0);


                    if (queue->count < MAX_TASKS) {
                        strcpy(queue->tasks[queue->rear], task);
                        queue->rear = (queue->rear + 1) % MAX_TASKS;
                        queue->count++;
                        signal_semaphore(sem_id, 1);
                    }

                    signal_semaphore(sem_id, 0);
                    sleep(rand() % 5 + 1);
                }
            } else {
                // Printer process
                int printer_id = i - numberOfUsers + 2;
                while (1) {
                    wait_semaphore(sem_id, 1);
                    wait_semaphore(sem_id, 0);

                    if (queue->count > 0) {
                        // Print task
                        char print_job[TASK_SIZE];
                        strcpy(print_job, queue->tasks[queue->front]);
                        queue->front = (queue->front + 1) % MAX_TASKS;
                        queue->count--;

                        signal_semaphore(sem_id, 0);

                        for (int j = 0; j < strlen(print_job); j++) {
                            printf("%c", print_job[j]);
                            fflush(stdout);
                            sleep(1);
                        }
                        printf("\n");
                    } else {
                        signal_semaphore(sem_id, 0);
                    }
                }
            }
            exit(0);
        }
    }

    for (int i = 0; i < numberOfUsers + numberOfPrinters; i++) {
        wait(NULL);
    }

    
    shmdt(queue);
    shmctl(shm_id, IPC_RMID, NULL);
    semctl(sem_id, 0, IPC_RMID);

    return 0;
}
