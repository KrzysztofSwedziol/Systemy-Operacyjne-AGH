#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#define FIFO_PATH "/home/krzysztof/CLionProjects/lab 6 pipes/myFIFO"

double calculateIntegral(double a, double b) {
    double result = 0.0;
    double width = 0.0001;
    for (double x = a; x < b; x += width) {
        result += 4.0 / (1.0 + x * x) * width;
    }
    return result;
}

int main(){
    int fd = open(FIFO_PATH, O_RDONLY);
    double a, b;
    read(fd, &a, sizeof(a));
    read(fd, &b, sizeof(b));

    double result = calculateIntegral(a, b);

    fd = open(FIFO_PATH, O_WRONLY);
    write(fd, &result, sizeof(result));
    close(fd);

    return 0;
}

