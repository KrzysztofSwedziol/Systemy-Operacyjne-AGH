#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>

#define BUFFER_SIZE 1024
#define MAX_CLIENTS 10
#define ALIVE_INTERVAL 10

typedef struct {
    char name[50];
    struct sockaddr_in addr;
    int alive;
} Client;

Client clients[MAX_CLIENTS];
int client_count = 0;
pthread_mutex_t client_mutex = PTHREAD_MUTEX_INITIALIZER;
int server_socket;

void *ping_clients(void *arg) {
    while (1) {
        sleep(ALIVE_INTERVAL);
        pthread_mutex_lock(&client_mutex);
        for (int i = 0; i < client_count; i++) {
            if (clients[i].alive == 0) {
                printf("Removing inactive client: %s\n", clients[i].name);
                clients[i] = clients[client_count - 1];
                client_count--;
                i--;
            } else {
                clients[i].alive = 0;
                sendto(server_socket, "PING", 4, 0, (struct sockaddr *)&clients[i].addr, sizeof(clients[i].addr));
            }
        }
        pthread_mutex_unlock(&client_mutex);
    }
    return NULL;
}

void handle_client(char *message, struct sockaddr_in *client_addr) {
    char response[BUFFER_SIZE];
    char command[BUFFER_SIZE];
    sscanf(message, "%s", command);

    if (strcmp(command, "REGISTER") == 0) {
        char name[50];
        sscanf(message, "REGISTER %s", name);
        pthread_mutex_lock(&client_mutex);
        if (client_count < MAX_CLIENTS) {
            strcpy(clients[client_count].name, name);
            clients[client_count].addr = *client_addr;
            clients[client_count].alive = 1;
            client_count++;
            printf("Client registered: %s\n", name);
        }
        pthread_mutex_unlock(&client_mutex);
    } else if (strcmp(command, "LIST") == 0) {
        pthread_mutex_lock(&client_mutex);
        strcpy(response, "Clients: ");
        for (int i = 0; i < client_count; i++) {
            strcat(response, clients[i].name);
            if (i < client_count - 1) {
                strcat(response, ", ");
            }
        }
        pthread_mutex_unlock(&client_mutex);
        sendto(server_socket, response, strlen(response), 0, (struct sockaddr *)client_addr, sizeof(*client_addr));
    } else if (strcmp(command, "2ALL") == 0) {
        char *msg = strchr(message, ' ') + 1;
        pthread_mutex_lock(&client_mutex);
        for (int i = 0; i < client_count; i++) {
            if (memcmp(&clients[i].addr, client_addr, sizeof(*client_addr)) != 0) {
                snprintf(response, BUFFER_SIZE, "%s: %s", clients[i].name, msg);
                sendto(server_socket, response, strlen(response), 0, (struct sockaddr *)&clients[i].addr, sizeof(clients[i].addr));
            }
        }
        pthread_mutex_unlock(&client_mutex);
    } else if (strcmp(command, "2ONE") == 0) {
        char recipient[50];
        char *msg = strchr(strchr(message, ' ') + 1, ' ') + 1;
        sscanf(message, "2ONE %s", recipient);
        pthread_mutex_lock(&client_mutex);
        for (int i = 0; i < client_count; i++) {
            if (strcmp(clients[i].name, recipient) == 0) {
                snprintf(response, BUFFER_SIZE, "%s: %s", clients[i].name, msg);
                sendto(server_socket, response, strlen(response), 0, (struct sockaddr *)&clients[i].addr, sizeof(clients[i].addr));
                break;
            }
        }
        pthread_mutex_unlock(&client_mutex);
    } else if (strcmp(command, "STOP") == 0) {
        pthread_mutex_lock(&client_mutex);
        for (int i = 0; i < client_count; i++) {
            if (memcmp(&clients[i].addr, client_addr, sizeof(*client_addr)) == 0) {
                printf("Client unregistered: %s\n", clients[i].name);
                clients[i] = clients[client_count - 1];
                client_count--;
                break;
            }
        }
        pthread_mutex_unlock(&client_mutex);
    } else if (strcmp(command, "ALIVE") == 0) {
        pthread_mutex_lock(&client_mutex);
        for (int i = 0; i < client_count; i++) {
            if (memcmp(&clients[i].addr, client_addr, sizeof(*client_addr)) == 0) {
                clients[i].alive = 1;
                break;
            }
        }
        pthread_mutex_unlock(&client_mutex);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <IP> <PORT>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *server_ip = argv[1];
    int server_port = atoi(argv[2]);

    server_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_socket < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(server_ip);
    server_addr.sin_port = htons(server_port);

    if (bind(server_socket, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    pthread_t ping_thread;
    pthread_create(&ping_thread, NULL, ping_clients, NULL);

    printf("Server started at %s:%d\n", server_ip, server_port);

    while (1) {
        char buffer[BUFFER_SIZE];
        struct sockaddr_in client_addr;
        socklen_t addr_len = sizeof(client_addr);
        int n = recvfrom(server_socket, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &addr_len);
        if (n > 0) {
            buffer[n] = '\0';
            handle_client(buffer, &client_addr);
        }
    }

    close(server_socket);
    return 0;
}
