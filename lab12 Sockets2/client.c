#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>

#define BUFFER_SIZE 1024

int client_socket;
struct sockaddr_in server_addr;
char client_name[50];

void *receive_messages(void *arg) {
    char buffer[BUFFER_SIZE];
    while (1) {
        int n = recvfrom(client_socket, buffer, BUFFER_SIZE, 0, NULL, NULL);
        if (n > 0) {
            buffer[n] = '\0';
            if (strcmp(buffer, "PING") == 0) {
                sendto(client_socket, "ALIVE", 5, 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
            } else {
                printf("%s\n", buffer);
            }
        }
    }
    return NULL;
}

void handle_exit(int sig) {
    char stop_command[BUFFER_SIZE];
    snprintf(stop_command, BUFFER_SIZE, "STOP %s", client_name);
    sendto(client_socket, stop_command, strlen(stop_command), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
    close(client_socket);
    printf("Unregistered and exiting...\n");
    exit(0);
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <NAME> <SERVER_IP> <SERVER_PORT>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    strcpy(client_name, argv[1]);
    const char *server_ip = argv[2];
    int server_port = atoi(argv[3]);

    client_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (client_socket < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(server_ip);
    server_addr.sin_port = htons(server_port);

    char register_command[BUFFER_SIZE];
    snprintf(register_command, BUFFER_SIZE, "REGISTER %s", client_name);
    sendto(client_socket, register_command, strlen(register_command), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));

    pthread_t receive_thread;
    pthread_create(&receive_thread, NULL, receive_messages, NULL);

    signal(SIGINT, handle_exit);

    char buffer[BUFFER_SIZE];
    while (1) {
        fgets(buffer, BUFFER_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = '\0'; // Remove newline character
        if (strcmp(buffer, "STOP") == 0) {
            handle_exit(SIGINT);
        } else {
            sendto(client_socket, buffer, strlen(buffer), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
        }
    }

    close(client_socket);
    return 0;
}
