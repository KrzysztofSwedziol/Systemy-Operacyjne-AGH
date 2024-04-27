#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>

#define MAX_CLIENTS 10


struct message_from_client{
    long type;
    char text[1024];
    int clientID;
    int clientQueueID;

};

struct message_to_client{
    long type;
    char text[1024];
    int clientID;
    int serverQueueID;
};

int client_queues[MAX_CLIENTS];
int client_count = 0;

int main() {
    printf("Starting server\n");

    key_t serverKey = ftok("/bin/ls", 'a');
    int server_queue_id = msgget(serverKey, IPC_CREAT | 0666);
    if (server_queue_id == -1) {
        perror("Server Queue Creation Failed");
        exit(1);
    }
    printf("Server's queue id is : %d\n", server_queue_id);

    struct message_from_client fromMsg;
    struct message_to_client toMsg;

    while(1){
        if(msgrcv(server_queue_id, &fromMsg, sizeof(fromMsg) - sizeof(long), 0, 0) != -1){
            if(fromMsg.type == 1){
                printf("Server received client's initializing message\n");
                if(client_count < MAX_CLIENTS){
                    client_queues[client_count] = fromMsg.clientQueueID;

                    toMsg.type = fromMsg.clientQueueID;
                    strcpy(toMsg.text, "Added you as a new client");
                    toMsg.clientID = client_count;
                    toMsg.serverQueueID = server_queue_id;
                    msgsnd(fromMsg.clientQueueID, &toMsg, sizeof(toMsg) - sizeof(long), 0);
                    printf("Initializing message was sent back to client\n");
                    client_count++;
                }

            }
            else if(fromMsg.type == 2){
                printf("Server received ordinary message from client\n");
                for(int i=0; i< MAX_CLIENTS; i++){
                    if(client_queues[i] != 0 && client_queues[i] != fromMsg.clientQueueID){
                        toMsg.serverQueueID = server_queue_id;
                        toMsg.clientID = i;
                        toMsg.type = client_queues[i];
                        char about_sender[300];
                        sprintf(about_sender, "%s This message was sent from client with ID of %d.", fromMsg.text, i);
                        strcpy(toMsg.text, about_sender);
                        msgsnd(client_queues[i], &toMsg, sizeof(toMsg) - sizeof(long), 0);
                        printf("Server sent received ordinary message to all other clients\n");
                    }
                }
            }
        }
    }

    return 0;
}
