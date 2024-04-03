#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>

double calculate_Part(double start, double end, double width){
    double sum = 0.0;
    double dist = end - start;
    double counter = 0;
    double curr = start;
    while(counter < dist){
        sum += 4.0 / (1.0 + curr * curr) * width;
        counter += width;
        curr += width;
    }
    return sum;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "use: %s <rectangle width> <amount of processes>\n", argv[0]);
        return 1;
    }

    struct timeval start, end;
    double elapsedTime;

    double width = atof(argv[1]);
    int processes = atoi(argv[2]);
    double range = 1.0 / processes;
    int pipefds[processes][2];
    pid_t pids[processes];
    double totalSum = 0.0;
    double temporary_sum = 0.0;

    gettimeofday(&start, NULL);

    for(int i=0; i<processes; i++){

        if(pipe(pipefds[i]) == -1){
            perror("pipe");
            exit(EXIT_FAILURE);
        }
        pids[i] = fork();

        if (pids[i] < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (pids[i] == 0){
            close(pipefds[i][0]);
            double start = i * range;
            double end = i * range + range;
            double result = calculate_Part(start, end, width);
            write(pipefds[i][1], &result, sizeof(result));
            close(pipefds[i][1]);
            exit(0);
        }else{
            close(pipefds[i][1]);
        }
    }
    for (int i = 0; i < processes; i++) {
        wait(NULL);
    }
    for(int i =0; i< processes; i++){
        read(pipefds[i][0], &temporary_sum, sizeof(temporary_sum));
        totalSum += temporary_sum;
        close(pipefds[i][0]);
    }
    gettimeofday(&end, NULL);
    printf("Total sum of given integral is : %f\n", totalSum);
    elapsedTime = (end.tv_sec - start.tv_sec) * 1000.0;      // sec to ms
    elapsedTime += (end.tv_usec - start.tv_usec) / 1000.0;   // us to ms

    printf("Elapsed time: %f ms\n", elapsedTime);

    FILE *file = fopen("raport_time.txt", "a");
    if (file == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    fprintf(file, "Processes: %d, Width: %f, Time: %f ms\n", processes, width, elapsedTime);
    fclose(file);

    return 0;
}
