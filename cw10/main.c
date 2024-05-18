#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

int reindeer_count = 0;
pthread_mutex_t santa_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t reindeer_count_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t all_reindeer_back = PTHREAD_COND_INITIALIZER;
pthread_cond_t delivery_finished = PTHREAD_COND_INITIALIZER;

pthread_mutex_t reindeers_mutexes[9] = {
    PTHREAD_MUTEX_INITIALIZER,
    PTHREAD_MUTEX_INITIALIZER,
    PTHREAD_MUTEX_INITIALIZER,
    PTHREAD_MUTEX_INITIALIZER,
    PTHREAD_MUTEX_INITIALIZER,
    PTHREAD_MUTEX_INITIALIZER,
    PTHREAD_MUTEX_INITIALIZER,
    PTHREAD_MUTEX_INITIALIZER,
    PTHREAD_MUTEX_INITIALIZER
};

pthread_t threads[10];

void* reindeer(void* ID){
    int id = *(int*)ID - 1;

    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

    while(1){
        //sleep(rand() % 6 + 5);
        sleep(rand() % 6 + 10);
        pthread_mutex_lock(&reindeer_count_mutex);
        reindeer_count++;
        printf("Renifer: czeka %d reniferów na Mikołaja, ID: %d\n", reindeer_count, id);

        if(reindeer_count == 9){
            printf("Renifer: wybudzam Mikołaja, ID: %d\n", id);
            pthread_cond_signal(&all_reindeer_back);
            reindeer_count = 0;
        }

        pthread_mutex_unlock(&reindeer_count_mutex);

        pthread_cond_wait(&delivery_finished, &reindeers_mutexes[id]);
        printf("Renifer: lecę na wakacje, ID: %d\n", id);
    }

    return NULL;
}

void* santa(){
    for(int i = 0; i < 4; i++){
        pthread_cond_wait(&all_reindeer_back, &santa_mutex);
        
        printf("Mikołaj: budzę się\n");
        printf("Mikołaj: dostarczam zabawki\n");
        
        sleep(rand() % 3 + 2);

        printf("Mikołaj: zasypiam\n");

        pthread_cond_broadcast(&delivery_finished);
    }

    for(int i = 1; i < 10; i++){
        pthread_cancel(threads[i]);
    }

    return NULL;
}

int main(){
    int ids[10];
    for(int i = 0; i < 10; i++){
        ids[i] = i;
    }

    pthread_create(&threads[0], NULL, santa, NULL);
    for(int i = 1; i < 10; i++){
        pthread_create(&threads[i], NULL, reindeer, &ids[i]);
    }

    for(int i = 0; i < 10; i++){
        pthread_join(threads[i], NULL);
    }

    return 0;
}