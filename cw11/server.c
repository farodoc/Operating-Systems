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

pthread_mutex_t lock;

void *ping_clients(void *arg) {
    int *client_socket = (int *)arg;
    char *ping_message = "ALIVE";
    while(1) {
        printf("Pinging users\n");
        for (int i = 0; i < MAX_CLIENTS; i++) {
            pthread_mutex_lock(&lock);
            if (client_socket[i] != 0) {
                send(client_socket[i], ping_message, strlen(ping_message), 0);
                struct timeval tv;
                tv.tv_sec = 1;
                tv.tv_usec = 0;
                setsockopt(client_socket[i], SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);

                char buffer[1025];
                int valread = read(client_socket[i], buffer, 1024);
                if (valread < 0) {
                    printf("Client %d did not respond to ALIVE message. Disconnecting...\n", i);
                    close(client_socket[i]);
                    client_socket[i] = 0;
                }
            }
            pthread_mutex_unlock(&lock);
        }
        sleep(5);
    }
    return NULL;
}

int main(int argc , char *argv[])
{
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <IP> <port>\n", argv[0]);
        return -1;
    }

    char *server_ip = argv[1];
    int server_port = atoi(argv[2]);

    int master_socket, addrlen, new_socket, client_socket[30], activity, valread, sd;
    int max_sd;
    struct sockaddr_in address;

    char buffer[1025];

    fd_set readfds;

    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        client_socket[i] = 0;
    }

    if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(server_port);

    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    printf("Listener on port %d \n", server_port);

    if (listen(master_socket, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    addrlen = sizeof(address);
    puts("Server running. Waiting for connections ...");

    pthread_mutex_init(&lock, NULL);
    pthread_t thread_id;
    pthread_create(&thread_id, NULL, ping_clients, (void*)client_socket);

    while(1) {
        FD_ZERO(&readfds);

        FD_SET(master_socket, &readfds);
        max_sd = master_socket;

        for (int i = 0 ; i < MAX_CLIENTS ; i++) {
            sd = client_socket[i];
        
            if(sd > 0)
                FD_SET(sd , &readfds);

            if(sd > max_sd)
                max_sd = sd;
        }

        activity = select(max_sd + 1 , &readfds , NULL , NULL , NULL);

        if ((activity < 0) && (errno!=EINTR)) {
            printf("select error");
        }

        // If something happened on the master socket, then its an incoming connection
        if (FD_ISSET(master_socket, &readfds)) {
            if ((new_socket = accept(master_socket, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
                perror("accept");
                exit(EXIT_FAILURE);
            }

            printf("New connection , socket fd is %d , ip is : %s , port : %d \n" , new_socket , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));

            char* message = "*** Welcome to server ***\n\n\n";
            if(send(new_socket, message, strlen(message), 0) != strlen(message)) {
                perror("send");
            }

            pthread_mutex_lock(&lock);
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if( client_socket[i] == 0 ) {
                    client_socket[i] = new_socket;
                    printf("Adding to list of sockets as %d\n" , i);
                    break;
                }
            }
            pthread_mutex_unlock(&lock);
        }

        for (int i = 0; i < MAX_CLIENTS; i++) {
            sd = client_socket[i];

            if (FD_ISSET(sd, &readfds)) {
                pthread_mutex_lock(&lock);
                if ((valread = read(sd, buffer, 1024)) == 0) {
                    // Somebody disconnected with CTRL+C
                    getpeername(sd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
                    printf("Host disconnected, ip %s, port %d \n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));

                    close(sd);
                    client_socket[i] = 0;
                }

                else {
                    buffer[valread] = '\0';

                    if (buffer[strlen(buffer) - 1] == '\n') {
                        buffer[strlen(buffer) - 1] = '\0';
                    }

                    // help command
                    if(strcmp(buffer, "help") == 0) {
                        char message[1024] = {0};
                        strcat(message, "LIST - list all users currently connected to the server\n");
                        strcat(message, "2ALL <message> - send message to all connected users\n");
                        strcat(message, "2ONE <user_id> <message> - send message to specified user\n");
                        strcat(message, "STOP - log out from the server\n");
                        send(sd, message, strlen(message), 0);
                        memset(message, 0, sizeof(message));
                    }

                    // LIST command
                    else if (strcmp(buffer, "LIST") == 0) {
                        char message[1024] = {0};

                        for (int j = 0; j < MAX_CLIENTS; j++) {
                            if (client_socket[j] != 0) {
                                char client_info[MAX_CLIENTS];
                                sprintf(client_info, "Client %d is active\n", j);
                                strcat(message, client_info);
                            }
                        }

                        send(sd, message, strlen(message), 0);
                        memset(message, 0, sizeof(message));
                    }

                    // 2ALL command
                    else if (strncmp(buffer, "2ALL ", 5) == 0) {
                        char* message = buffer + 5;

                        time_t now = time(NULL);
                        char* datetime = ctime(&now);

                        char all_message[1025];
                        sprintf(all_message, "Client %d at %s>> %s\n", i, datetime, message);

                        for (int j = 0; j < MAX_CLIENTS; j++) {
                            if (client_socket[j] != 0 && j != i) {
                                send(client_socket[j], all_message, strlen(all_message), 0);
                            }
                        }
                    }

                    // 2ONE command
                    else if (strncmp(buffer, "2ONE ", 5) == 0) {
                        int client_id = atoi(buffer + 5);

                        char* next_space = strchr(buffer + 5, ' ');
                        if (next_space == NULL) {
                            printf("Invalid 2ONE command format\n");
                            continue;
                        }

                        char* message = next_space + 1;

                        time_t now = time(NULL);
                        char* datetime = ctime(&now);

                        char one_message[1025];
                        sprintf(one_message, "From client %d at %s>> %s\n", i, datetime, message);

                        if (client_id >= 0 && client_id < MAX_CLIENTS && client_socket[client_id] != 0) {
                            send(client_socket[client_id], one_message, strlen(one_message), 0);
                        } else {
                            memset(one_message, 0, sizeof(one_message));
                            sprintf(one_message, "Invalid client ID\n");
                            printf("Invalid client ID\n");
                        }
                    }

                    // STOP command
                    else if (strcmp(buffer, "STOP") == 0) {
                        close(sd);
                        client_socket[i] = 0;
                        printf("Client %d disconnected\n", i);
                    }
                }
                memset(buffer, 0, sizeof(buffer));
                pthread_mutex_unlock(&lock);
            }
        }
    }

    pthread_mutex_destroy(&lock);
    return 0;
} 