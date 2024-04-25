#include <stdio.h>
#include <fcntl.h>
#include <mqueue.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "server_specs.h"

int main() {
    struct mq_attr attr = {
        .mq_flags = 0,
        .mq_maxmsg = MAX_MESSAGES,
        .mq_msgsize = sizeof(message),
        .mq_curmsgs = 0
    };

    pid_t pid = getpid();
    char client_queue_name[20];
    sprintf(client_queue_name, "/chat_queue_%d", pid);

    mqd_t client_mq = mq_open(client_queue_name, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, &attr);
    if(client_mq == -1) {
        perror("mq_open client");
        return -1;
    }

    mqd_t server_mq = mq_open(QUEUE_NAME, O_RDWR, S_IRUSR | S_IWUSR, NULL);
    if(server_mq == -1) {
        perror("mq_open server");
        return -1;
    }

    message msg;
    msg.type = INIT;
    msg.identifier = -1;
    strncpy(msg.text, client_queue_name, sizeof(msg.text));

    mq_send(server_mq, (char*)&msg, sizeof(msg), 0);

    int fd[2];

    if(pipe(fd) < 0){
        perror("pipe");
        return -1;
    }

    pid_t listener = fork();

    switch(listener){
        case -1:
            perror("fork");
            return -1;

        case 0:
            close(fd[0]);
            message received_msg;

            FILE* file = NULL;

            while(1){
                mq_receive(client_mq, (char*)&received_msg, sizeof(received_msg), NULL);

                switch(received_msg.type){
                    case IDENTIFIER:
                        printf("Your identifier is %d\n", received_msg.identifier);

                        char filename[20];
                        sprintf(filename, "client%d.txt", received_msg.identifier);
                        file = fopen(filename, "a");
                        if (file == NULL) {
                            perror("fopen");
                            return -1;
                        }

                        write(fd[1], &received_msg.identifier, sizeof(received_msg.identifier));
                        break;

                    case MESSAGE:
                        fprintf(file, "Received msg from client %d: %s\n", received_msg.identifier, received_msg.text);
                        fflush(file);
                        break;


                    default:
                        printf("Unexpected msg type\n");
                        break;
                }
            }

            fclose(file);
        
        default:
            close(fd[1]);
            int identifier = -1;
            read(fd[0], &identifier, sizeof(identifier));

            char* buffer = malloc(MESSAGE_SIZE);
            
            while(1){
                printf("Enter msg: ");
                fgets(buffer, MESSAGE_SIZE, stdin);

                message send_msg;

                send_msg.type = MESSAGE;
                send_msg.identifier = identifier;
                strncpy(send_msg.text, buffer, sizeof(send_msg.text));

                mq_send(server_mq, (char*)&send_msg, sizeof(send_msg), 0);

                free(buffer);
                buffer = malloc(MESSAGE_SIZE);
            }
    }

    return 0;
}