#include <ncurses.h>
#include <locale.h>
#include <unistd.h>
#include <stdbool.h>
#include "grid.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <math.h>
#include <signal.h>


#define THREAD_COUNT 12


typedef struct {
    int start;
    int end;

    char** background;
    char** foreground;
} thread_args_t;

void signal_handler(int signum) {
    // Just a handler to catch the signal and return
}

void* thread_function(void* arg){
    thread_args_t* args = (thread_args_t*)arg;

    while(true){
        pause();

        for(int i = args->start; i < args->end; i++){
            int row = i / GRID_WIDTH;
            int col = i % GRID_WIDTH;

            (*args->background)[i] = is_alive(row, col, *args->foreground);
        }
    }

}

int main()
{
    srand(time(NULL));
    setlocale(LC_CTYPE, "");
    initscr(); // Start curses mode

    char *foreground = create_grid();
    char *background = create_grid();
    char *tmp;

    init_grid(foreground);


    pthread_t threads[THREAD_COUNT];
    thread_args_t args[THREAD_COUNT];
    int area = GRID_HEIGHT * GRID_WIDTH;
    int single_thread_job = area/THREAD_COUNT;
    int remaining = area%THREAD_COUNT;


    struct sigaction sa;
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGUSR1, &sa, NULL);


    for(int i = 0; i < THREAD_COUNT; i++){
        if(i == (THREAD_COUNT - 1)){                //last thread
            int start_index = i * single_thread_job;
            int end_index = (i+1) * single_thread_job + remaining;
            args[i].start = start_index;
            args[i].end = end_index;

        }else{
            int start_index = i * single_thread_job;
            int end_index = (i+1) * single_thread_job;
            args[i].start = start_index;
            args[i].end = end_index;

        }
        args[i].background = &background;
        args[i].foreground = &foreground;
        pthread_create(&threads[i], NULL, thread_function, &args[i]);
    }

    while(true){
        draw_grid(foreground);

        for(int i = 0; i < THREAD_COUNT; i++) {
            pthread_kill(threads[i], SIGUSR1);
        }

        usleep(500 * 1000);

        // Step simulation
        //update_grid(foreground, background);

        tmp = foreground;
        foreground = background;
        background = tmp;
    }

    endwin(); // End curses mode
    destroy_grid(foreground);
    destroy_grid(background);

    return 0;
}
