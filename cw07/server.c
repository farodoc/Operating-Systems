#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>

#include "server_specs.h"

int main() {
    struct mq_attr attr = {
        .mq_flags = 0,
        .mq_maxmsg = MAX_MESSAGES,
        .mq_msgsize = sizeof(message),
        .mq_curmsgs = 0
    };

    mqd_t mq = mq_open(QUEUE_NAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, &attr);

    if(mq == -1) {
        perror("mq_open");
        return -1;
    }
    
    message msg;

    mqd_t client_mq[MAX_CLIENTS];
    int client_count = 0;

    while(1) {
        mq_receive(mq, (char*)&msg, sizeof(msg), NULL);

        switch(msg.type) {
            case INIT:
                client_mq[client_count] = mq_open(msg.text, O_RDWR, S_IRUSR | S_IWUSR, NULL);
                if(client_mq[client_count] == -1) {
                    perror("mq_open");
                    return -1;
                }

                msg.type = IDENTIFIER;
                msg.identifier = client_count;

                mq_send(client_mq[client_count], (char*)&msg, sizeof(msg), 10);

                client_count++;
                break;

            case MESSAGE:
                for(int i = 0; i < client_count; i++) {
                    if(i != msg.identifier)
                        mq_send(client_mq[i], (char*)&msg, sizeof(msg), 10);
                }
                break;

            default:
                printf("Unexpected message type\n");
                break;
        }
    }

    return 0;
}