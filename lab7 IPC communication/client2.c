#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <unistd.h>



struct message_to_server {
    long type;
    char text[1024];
    int clientID;
    int clientQueueID;
};

struct message_from_server {
    long type;
    char text[1024];
    int clientID;
    int serverQueueID;
};

void receive_messages(int my_queue_id) {
    struct message_from_server msg;
    while (1) {

        if (msgrcv(my_queue_id, &msg, sizeof(msg) - sizeof(long), my_queue_id, 0) == -1) {
            perror("Error receiving message");
            exit(1);
        }
        printf("Received: %s\n", msg.text);
    }
}

int main() {
    printf("Starting client\n");

    key_t clientKey = ftok("/bin/ls", 'c');
    key_t serverKey = ftok("/bin/ls", 'a');
    if (clientKey == -1) {
        perror("Failed to generate key");
        exit(1);
    }
    printf("Key: %d\n", clientKey);


    int clientQueueID = msgget(clientKey, IPC_CREAT | 0666);
    if (clientQueueID == -1) {
        perror("Failed to get client message queue");
        exit(1);
    }
    printf("Client's queue id is : %d\n", clientQueueID);


    int serverQueueID = msgget(serverKey, 0);
    if (serverQueueID == -1) {
        perror("Failed to connect to server");
        exit(1);
    }
    printf("server's queue id is : %d\n", serverQueueID);

    struct message_to_server toServer = {1, "INIT", 0, clientQueueID};
    struct message_from_server fromServer;


    if (msgsnd(serverQueueID, &toServer, sizeof(toServer) - sizeof(long), 0) == -1) {
        perror("Failed to send initial message to server");
        exit(1);
    }else{
        printf("message was sent\n");
    }


    if (msgrcv(clientQueueID, &fromServer, sizeof(fromServer) - sizeof(long), clientQueueID, 0) == -1) {
        perror("Failed to receive message from server");
        exit(1);
    }
    printf("Received message from server: %s\n", fromServer.text);


    pid_t pid = fork();
    if (pid == 0) {

        receive_messages(clientQueueID);
    } else {

        while (fgets(toServer.text, sizeof(toServer.text), stdin)) {
            toServer.type = 2;
            if (msgsnd(serverQueueID, &toServer, sizeof(toServer) - sizeof(long), 0) == -1) {
                perror("Failed to send message to server");
                exit(1);
            }
        }
    }

    return 0;
}
