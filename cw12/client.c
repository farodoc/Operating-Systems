#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "server_spec.h"

volatile int server_alive = 1;

void *receive_messages(void *socket) {
    int sock = *(int *)socket;
    struct sockaddr_in server_address;
    socklen_t server_address_len = sizeof(server_address);
    char buffer[BUFFER_SIZE];
    while(server_alive) {
        memset(buffer, 0, sizeof(buffer));
        int recv_result = recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr*)&server_address, &server_address_len);

        if(recv_result < 0)
        {
            puts("recvfrom failed");
            break;
        }

        if (strncmp(buffer, "PING", 5) == 0) {
            char *response = "PING_RESPONSE";
            sendto(sock, response, strlen(response), 0, (struct sockaddr*)&server_address, server_address_len);
        } else if (strncmp(buffer, "STOP", 4) == 0) {
            server_alive = 0;
            puts("Exiting...");
        } else {
            puts("Server reply:");
            puts(buffer);
        }
    }
    return NULL;
}


int main(int argc , char *argv[])
{
    if(argc != 3) {
        printf("Usage: %s <client_id> <server_address:port>\n", argv[0]);
        return -1;
    }

    char client_id[MAX_ID_LENGTH];
    strncpy(client_id, argv[1], MAX_ID_LENGTH - 1);
    client_id[MAX_ID_LENGTH - 1] = '\0';

    char *server_address = strtok(argv[2], ":");
    char *server_port_str = strtok(NULL, ":");

    if (server_address == NULL || server_port_str == NULL) {
        printf("\nInvalid server address. It should be in the format IP:PORT\n");
        return -1;
    }

    int server_port = atoi(server_port_str);
    if (server_port == 0) {
        printf("\nInvalid port number\n");
        return -1;
    }

    int sock = 0;
    struct sockaddr_in serv_addr;
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        printf("\nSocket creation error\n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(server_port);

    if(inet_pton(AF_INET, server_address, &serv_addr.sin_addr)<=0) 
    {
        printf("\nInvalid address/Address not supported\n");
        return -1;
    }

    pthread_t thread_id;
    if(pthread_create(&thread_id, NULL, receive_messages, (void *)&sock) < 0)
    {
        perror("could not create thread");
        return -1;
    }

    char buffer[BUFFER_SIZE] = {0};

    char *init_message = "";
    socklen_t server_address_len = sizeof(serv_addr);
    sendto(sock, init_message, strlen(init_message), 0, (struct sockaddr*)&serv_addr, server_address_len);

    while(server_alive) {
        fgets(buffer, sizeof(buffer), stdin);
        printf("\n");

        if(server_alive && (sendto(sock , buffer , strlen(buffer) , 0, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0))
        {
            puts("Send failed");
            return -1;
        }

        memset(buffer, 0, sizeof(buffer));
    }
    
    return 0;
}