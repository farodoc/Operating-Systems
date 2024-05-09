#ifndef SHM_SPEC_H
#define SHM_SPEC_H

#include <semaphore.h>

#define SHM_NAME "printers_shm"
#define MAX_PRINTERS 10
#define MAX_PRINTER_BUFFER 10
#define MAX_PRINTER_NAME 20
#define MAX_TASKS 5

typedef enum {
    PRINTER_FREE,
    PRINTER_BUSY
} printer_status_t;

typedef struct {
    char buffer[MAX_PRINTER_BUFFER];
    int user_id;
} task_t;

typedef struct {
    sem_t printer_sem;
    char name[MAX_PRINTER_NAME];
    char buffer[MAX_PRINTER_BUFFER];
    size_t buffer_size;
    printer_status_t status;
} printer_t;

typedef struct {
    printer_t printers[MAX_PRINTERS];
    task_t tasks[MAX_TASKS];
    int start, end;
    int printers_count;
    sem_t tasks_sem;
} printers_t;

#endif // SHM_SPEC_H