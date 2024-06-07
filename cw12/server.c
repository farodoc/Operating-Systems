#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>

#include "server_spec.h"

client_info clients[MAX_CLIENTS];

void check_clients() {
    for(int i = 0; i < MAX_CLIENTS; i++) {
        if(clients[i].active && clients[i].last_ping_time != -1 && (time(NULL) - clients[i].last_response_time > TIMEOUT)) {
            clients[i].active = 0;
            printf("Client %d timed out\n", i);
        }
    }
}

void *ping_clients(void *arg) {
    int server_socket = *(int *)arg;
    while(1) {
        printf("Pinging clients\n");
        for(int i = 0; i < MAX_CLIENTS; i++) {
            if(clients[i].active) {
                char *ping_message = "PING";
                sendto(server_socket, ping_message, strlen(ping_message), 0, (struct sockaddr*)&clients[i].address, sizeof(clients[i].address));
                clients[i].last_ping_time = time(NULL);
            }
        }
        sleep(RESPONSE_TIME);
        check_clients();
        sleep(PING_INTERVAL);
    }
    return NULL;
}

int main(int argc , char *argv[])
{
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <IP> <port>\n", argv[0]);
        return -1;
    }

    int server_port = atoi(argv[2]);

    int server_socket;
    struct sockaddr_in server_address;

    char buffer[BUFFER_SIZE];

    for (int i = 0; i < MAX_CLIENTS; i++) {
        clients[i].active = 0;
        clients[i].last_ping_time = -1;
    }

    if ((server_socket = socket(AF_INET , SOCK_DGRAM , 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(server_port);

    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address))<0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    printf("Listener on port %d \n", server_port);

    pthread_t thread_id;
    if(pthread_create(&thread_id, NULL, ping_clients, (void *)&server_socket) < 0) {
        perror("thread error");
        exit(EXIT_FAILURE);
    }

    while(1) {
        struct sockaddr_in client_address;
        socklen_t client_address_len = sizeof(client_address);
        int valread = recvfrom(server_socket, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&client_address, &client_address_len);

        if(valread < 0) {
            perror("recvfrom failed");
            continue;
        }

        buffer[valread] = '\0';

        if(buffer[strlen(buffer) - 1] == '\n') {
            buffer[strlen(buffer) - 1] = '\0';
        }

        int client_id = -1;
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i].active && clients[i].address.sin_addr.s_addr == client_address.sin_addr.s_addr && clients[i].address.sin_port == client_address.sin_port) {
                client_id = i;
                break;
            }
        }

        if(client_id == -1) {
            for(int i = 0; i < MAX_CLIENTS; i++) {
                if(!clients[i].active) {
                    clients[i].active = 1;
                    clients[i].address = client_address;
                    client_id = i;
                    printf("Client %d connected\n", i);
                    break;
                }
            }
        }

        if(client_id == -1) {
            printf("Server is full.\n");
            continue;
        }

        // LIST command
        if(strcmp(buffer, "LIST") == 0) {
            char message[BUFFER_SIZE] = {0};

            for(int j = 0; j < MAX_CLIENTS; j++) {
                if(clients[j].active) {
                    char client_info[50];
                    sprintf(client_info, "Client %d is active\n", j);
                    strcat(message, client_info);
                }
            }

            sendto(server_socket, message, strlen(message), 0, (struct sockaddr*)&client_address, client_address_len);
        }

        // 2ALL command
        else if(strncmp(buffer, "2ALL ", 5) == 0) {
            char* message = buffer + 5;

            char all_message[BUFFER_SIZE];
            sprintf(all_message, "Client %d >> %s\n", client_id, message);

            for(int j = 0; j < MAX_CLIENTS; j++) {
                if(clients[j].active && j != client_id) {
                    sendto(server_socket, all_message, strlen(all_message), 0, (struct sockaddr*)&clients[j].address, sizeof(clients[j].address));
                }
            }
        }

        // 2ONE command
        else if(strncmp(buffer, "2ONE ", 5) == 0) {
            int target_client_id = atoi(buffer + 5);

            char* next_space = strchr(buffer + 5, ' ');
            if(next_space == NULL) {
                printf("Invalid 2ONE command format\n");
                continue;
            }

            char* message = next_space + 1;

            char one_message[BUFFER_SIZE];
            sprintf(one_message, "From client %d >> %s\n", client_id, message);

            if(target_client_id >= 0 && target_client_id < MAX_CLIENTS && clients[target_client_id].active) {
                sendto(server_socket, one_message, strlen(one_message), 0, (struct sockaddr*)&clients[target_client_id].address, sizeof(clients[target_client_id].address));
            } else {
                printf("Invalid client ID\n");
            }
        }

        // STOP command
        else if(strcmp(buffer, "STOP") == 0) {
            clients[client_id].active = 0;
            char *stop_message = "STOP";
            sendto(server_socket, stop_message, strlen(stop_message), 0, (struct sockaddr*)&clients[client_id].address, sizeof(clients[client_id].address));
            printf("Client %d disconnected\n", client_id);
        }

        else if(strcmp(buffer, "PING_RESPONSE") == 0) {
            clients[client_id].last_response_time = time(NULL);
        }
    }

    return 0;
} 