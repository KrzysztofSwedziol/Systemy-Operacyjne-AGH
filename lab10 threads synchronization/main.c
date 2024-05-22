#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_REINDEER 9
#define NUM_DELIVERIES 4

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

int reindeer_count = 0;
int deliveries = 0;

void* santa_thread(void* arg) {
    while (1) {
        pthread_mutex_lock(&mutex);

        while (reindeer_count < NUM_REINDEER) {
            pthread_cond_wait(&cond, &mutex);
        }

        if (deliveries >= NUM_DELIVERIES) {
            pthread_mutex_unlock(&mutex);
            break;
        }

        printf("Mikołaj: budzę się\n");
        printf("Mikołaj: dostarczam zabawki\n");
        sleep(rand() % 3 + 2);
        deliveries++;
        reindeer_count = 0;
        pthread_cond_broadcast(&cond);
        printf("Mikołaj: zasypiam\n");

        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}


void* reindeer_thread(void* arg) {
    int id = *((int*)arg);
    free(arg);

    while (1) {
        sleep(rand() % 6 + 5);

        pthread_mutex_lock(&mutex);
        reindeer_count++;
        printf("Renifer: czeka %d reniferów na Mikołaja, %d\n", reindeer_count, id);

        if (reindeer_count == NUM_REINDEER) {
            printf("Renifer: wybudzam Mikołaja, %d\n", id);
            pthread_cond_signal(&cond);
        }

        while (reindeer_count != 0) {
            pthread_cond_wait(&cond, &mutex);
        }

        if (deliveries >= NUM_DELIVERIES) {
            pthread_mutex_unlock(&mutex);
            break;
        }

        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

int main() {
    srand(time(NULL));
    pthread_t santa;
    pthread_t reindeers[NUM_REINDEER];

    pthread_create(&santa, NULL, santa_thread, NULL);

    for (int i = 0; i < NUM_REINDEER; i++) {
        int* id = malloc(sizeof(int));
        *id = i + 1;
        pthread_create(&reindeers[i], NULL, reindeer_thread, id);
    }

    pthread_join(santa, NULL);

    for (int i = 0; i < NUM_REINDEER; i++) {
        pthread_join(reindeers[i], NULL);
    }

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);

    return 0;
}
