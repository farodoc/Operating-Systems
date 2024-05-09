#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#include "shm_specs.h"

void generate_random_text(char* buffer){
    for(int i = 0; i < MAX_PRINTER_BUFFER; i++){
        buffer[i] = 'a' + rand() % 26;
    }
    buffer[MAX_PRINTER_BUFFER] = '\0';
}

int main(int argc, char** argv){
    if(argc < 2){
        printf("Usage: %s <users_count>\n", argv[0]);
        return -1;
    }

    int users_count = atoi(argv[1]);

    int shm_fd = shm_open(SHM_NAME, O_RDWR, S_IRUSR | S_IWUSR);
    if(shm_fd == -1){
        perror("shm_open");
        return -1;
    }

    printers_t* printers = mmap(NULL, sizeof(printers_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if(printers == MAP_FAILED){
        perror("mmap");
        return -1;
    }

    char user_buffer[MAX_PRINTER_BUFFER];

    for(int i = 0; i < users_count; i++){
        pid_t user_pid = fork();
        if(user_pid == -1){
            perror("fork");
            return -1;
        }
        else if(user_pid == 0){
            srand(getpid());
            while(1){
                generate_random_text(user_buffer);

                int task_added = 0;
                do {
                    sem_wait(&printers->tasks_sem);
                    if((printers->end + 1) % MAX_TASKS != printers->start){
                        printers->tasks[printers->end].user_id = i;
                        strcpy(printers->tasks[printers->end].buffer, user_buffer);
                        printers->end = (printers->end + 1) % MAX_TASKS;
                        task_added = 1;
                        printf("User %d added task, \"%s\"\n", i, user_buffer);
                    }
                    sem_post(&printers->tasks_sem);
                } while (!task_added);

                sleep(1 + rand() % 5);
            }

            return 0;
        }
    }

    while(wait(NULL) > 0);

    munmap(printers, sizeof(printers_t));
}