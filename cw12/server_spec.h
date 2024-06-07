#ifndef SERVER_SPEC_H
#define SERVER_SPEC_H

#define MAX_CLIENTS 30
#define MAX_ID_LENGTH 100
#define BUFFER_SIZE 1025
#define PING_INTERVAL 10
#define RESPONSE_TIME 1
#define TIMEOUT 2

typedef struct {
    struct sockaddr_in address;
    int active;
    time_t last_ping_time;
    time_t last_response_time;
} client_info;

#endif // SERVER_SPEC_H