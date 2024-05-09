#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#include "shm_specs.h"

int main(int argc, char** argv){
    if(argc < 2){
        printf("Usage: %s <printers_count>\n", argv[0]);
        return -1;
    }

    int printers_count = atoi(argv[1]);

    if(printers_count > MAX_PRINTERS){
        printf("Printers count must be less than or equal to %d\n", MAX_PRINTERS);
        return -1;
    }

    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if(shm_fd == -1){
        perror("shm_open");
        return -1;
    }

    if(ftruncate(shm_fd, sizeof(printers_t)) == -1){
        perror("ftruncate");
        return -1;
    }

    printers_t* printers = mmap(NULL, sizeof(printers_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if(printers == MAP_FAILED){
        perror("mmap");
        return -1;
    }

    memset(printers, 0, sizeof(printers_t));
    printers->printers_count = printers_count;
    sem_init(&printers->tasks_sem, 1, 1);

    for(int i = 0; i < printers_count; i++){
        sem_init(&printers->printers[i].printer_sem, 1, 1);
        
        pid_t printer_pid = fork();
        if(printer_pid == -1){
            perror("fork");
            return -1;
        }
        else if(printer_pid == 0){
            while(1){
                if(printers->printers[i].status == PRINTER_FREE){
                    sem_wait(&printers->tasks_sem);
                    if(printers->start != printers->end){
                        strcpy(printers->printers[i].buffer, printers->tasks[printers->start].buffer);
                        printers->printers[i].buffer_size = strlen(printers->tasks[printers->start].buffer);
                        printers->start = (printers->start + 1) % MAX_TASKS;
                        printers->printers[i].status = PRINTER_BUSY;
                    }
                    sem_post(&printers->tasks_sem);
                }

                if(printers->printers[i].status == PRINTER_BUSY){
                    for(int j = 0; j < printers->printers[i].buffer_size; j++){
                        printf("%c", printers->printers[i].buffer[j]);
                        sleep(1);
                    }

                    printf("\n");
                    fflush(stdout);

                    printers->printers[i].status = PRINTER_FREE;
                    sem_post(&printers->printers[i].printer_sem);
                }
            }

            return 0;
        }
    }

    while(wait(NULL) > 0);

    for(int i = 0; i < printers_count; i++){
        sem_destroy(&printers->printers[i].printer_sem);
    }

    sem_destroy(&printers->tasks_sem);

    munmap(printers, sizeof(printers_t));

    shm_unlink(SHM_NAME);
}