#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>

#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

typedef struct {
    int socket;
    char id[32];
    int active;
} Client;

Client clients[MAX_CLIENTS];
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

void *handle_client(void *arg);
void send_message_to_all(char *message, int sender);
void send_message_to_one(char *message, int sender, char *receiver_id);
void list_clients(int client_socket);
void remove_client(int client_socket);

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <address> <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *address = argv[1];
    int port = atoi(argv[2]);

    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_size = sizeof(client_addr);

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(address);
    server_addr.sin_port = htons(port);

    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, 10) < 0) {
        perror("Listen failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on %s:%d\n", address, port);

    while (1) {
        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_size);
        if (client_socket < 0) {
            perror("Accept failed");
            continue;
        }

        pthread_t thread;
        pthread_create(&thread, NULL, handle_client, (void*)&client_socket);
    }

    close(server_socket);
    return 0;
}

void *handle_client(void *arg) {
    int client_socket = *(int*)arg;
    char buffer[BUFFER_SIZE];
    char client_id[32];
    int i;

    recv(client_socket, client_id, sizeof(client_id), 0);

    pthread_mutex_lock(&clients_mutex);
    for (i = 0; i < MAX_CLIENTS; ++i) {
        if (!clients[i].active) {
            clients[i].socket = client_socket;
            strcpy(clients[i].id, client_id);
            clients[i].active = 1;
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);

    if (i == MAX_CLIENTS) {
        printf("Max clients reached. Disconnecting %s\n", client_id);
        close(client_socket);
        return NULL;
    }

    printf("%s connected\n", client_id);

    while (1) {
        int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
        if (bytes_received <= 0) {
            printf("%s disconnected\n", client_id);
            break;
        }

        buffer[bytes_received] = '\0';
        char command[5];
        sscanf(buffer, "%4s", command);

        if (strcmp(command, "LIST") == 0) {
            list_clients(client_socket);
        } else if (strcmp(command, "2ALL") == 0) {
            send_message_to_all(buffer + 5, i);
        } else if (strcmp(command, "2ONE") == 0) {
            char receiver_id[32];
            sscanf(buffer + 5, "%s", receiver_id);
            send_message_to_one(buffer + 5 + strlen(receiver_id) + 1, i, receiver_id);
        } else if (strcmp(command, "STOP") == 0) {
            printf("%s disconnected\n", client_id);
            break;
        }
    }

    pthread_mutex_lock(&clients_mutex);
    clients[i].active = 0;
    pthread_mutex_unlock(&clients_mutex);

    close(client_socket);
    return NULL;
}

void list_clients(int client_socket) {
    char list[BUFFER_SIZE] = "Active clients:\n";
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients[i].active) {
            strcat(list, clients[i].id);
            strcat(list, "\n");
        }
    }
    pthread_mutex_unlock(&clients_mutex);
    send(client_socket, list, strlen(list), 0);
}

void send_message_to_all(char *message, int sender) {
    char buffer[BUFFER_SIZE];
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    snprintf(buffer, sizeof(buffer), "%02d:%02d:%02d %s: %s\n", t->tm_hour, t->tm_min, t->tm_sec, clients[sender].id, message);

    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients[i].active && i != sender) {
            send(clients[i].socket, buffer, strlen(buffer), 0);
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

void send_message_to_one(char *message, int sender, char *receiver_id) {
    char buffer[BUFFER_SIZE];
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    snprintf(buffer, sizeof(buffer), "%02d:%02d:%02d %s: %s\n", t->tm_hour, t->tm_min, t->tm_sec, clients[sender].id, message);

    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients[i].active && strcmp(clients[i].id, receiver_id) == 0) {
            send(clients[i].socket, buffer, strlen(buffer), 0);
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}
