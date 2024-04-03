#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#define FIFO_PATH "/home/krzysztof/CLionProjects/lab 6 pipes/myFIFO"

int main(int argc, char *argv[]){
    if (argc != 3) {
        fprintf(stderr, "use: %s <rectangle width> <amount of processes>\n", argv[0]);
        return 1;
    }

    double start = atof(argv[1]);
    double end = atof(argv[2]);

    mkfifo(FIFO_PATH, 0666);

    int fd = open(FIFO_PATH, O_WRONLY);
    write(fd, &start, sizeof(start));
    write(fd, &end, sizeof(end));

    close(fd);

    fd = open(FIFO_PATH, O_RDONLY);
    double result;
    read(fd, &result, sizeof(result));
    printf("Result of integration: %lf\n", result);
    close(fd);

    return 0;
}