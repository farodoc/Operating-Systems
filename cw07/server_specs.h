#ifndef SERVER_SPECS_H
#define SERVER_SPECS_H

#define MESSAGE_SIZE 1024
#define MAX_MESSAGES 10
#define MAX_CLIENTS 10
#define QUEUE_NAME "/server_chat_queue"

typedef enum{
    INIT,
    IDENTIFIER,
    MESSAGE
} message_type;

typedef struct {
    message_type type;
    int identifier;
    char text[MESSAGE_SIZE];
} message;


#endif // SERVER_SPECS_H