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
            while(1){
                int printer_idx = -1;
                for(int j = 0; j < printers->printers_count; j++){
                    int val;
                    sem_getvalue(&printers->printers[j].printer_sem, &val);
                    if(val == 1){
                        printer_idx = j;
                        break;
                    }
                }

                if(printer_idx == -1){
                    printer_idx = rand() % printers->printers_count;
                }

                if(sem_wait(&printers->printers[printer_idx].printer_sem) == -1){
                    perror("sem_wait");
                    return -1;
                }

                generate_random_text(user_buffer);

                strcpy(printers->printers[printer_idx].buffer, user_buffer);
                printers->printers[printer_idx].buffer_size = strlen(user_buffer);

                printers->printers[printer_idx].status = PRINTER_BUSY;

                printf("User %d using printer %d\n", i, printer_idx);
                fflush(stdout);

                sleep(1 + rand() % 5);
            }

            return 0;
        }
    }

    while(wait(NULL) > 0);

    munmap(printers, sizeof(printers_t));
}