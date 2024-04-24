#ifndef SERVER_SPECS_H
#define SERVER_SPECS_H

#define MESSAGE_SIZE 1024
#define MAX_MESSAGES 10
#define MAX_CLIENTS 5
#define QUEUE_NAME "/chat_queue"

typedef enum{
    INIT,
    IDENTIFIER,
    MESSAGE
} msg_type;

typedef struct {
    msg_type type;
    int identifier;
    char text[MESSAGE_SIZE];
} message;


#endif // SERVER_SPECS_H