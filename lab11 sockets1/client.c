#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>

#define BUFFER_SIZE 1024

int sockfd;
char client_id[32];

void *receive_messages(void *arg);
void handle_sigint(int sig);

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <client_id> <server_address> <server_port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    strcpy(client_id, argv[1]);
    char *server_address = argv[2];
    int server_port = atoi(argv[3]);

    struct sockaddr_in server_addr;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(server_address);
    server_addr.sin_port = htons(server_port);

    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection to server failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    send(sockfd, client_id, strlen(client_id), 0);

    pthread_t thread;
    pthread_create(&thread, NULL, receive_messages, NULL);

    signal(SIGINT, handle_sigint);

    char buffer[BUFFER_SIZE];
    while (1) {
        fgets(buffer, BUFFER_SIZE, stdin);
        if (strcmp(buffer, "STOP\n") == 0) {
            send(sockfd, "STOP", strlen("STOP"), 0);
            break;
        }
        send(sockfd, buffer, strlen(buffer), 0);
    }

    close(sockfd);
    return 0;
}

void *receive_messages(void *arg) {
    char buffer[BUFFER_SIZE];
    while (1) {
        int bytes_received = recv(sockfd, buffer, sizeof(buffer), 0);
        if (bytes_received <= 0) {
            printf("Disconnected from server\n");
            exit(EXIT_FAILURE);
        }
        buffer[bytes_received] = '\0';
        printf("%s", buffer);
    }
    return NULL;
}

void handle_sigint(int sig) {
    send(sockfd, "STOP", strlen("STOP"), 0);
    close(sockfd);
    exit(EXIT_SUCCESS);
}
