#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define BLOCK_SIZE 1024

void copy_file_by_chars(FILE* from, FILE* to){
    #ifdef BYBLOCKS
    return;
    #endif

    char c;

    fseek(from, -1, SEEK_END);
    long long file_size = ftell(from) + 1;

    int i = 0;

    while(i < file_size){
        c = fgetc(from);
        fputc(c, to);
        fseek(from, -2, SEEK_CUR);
        i++;
    }
}


void copy_file_by_blocks(FILE* from, FILE* to){
    #ifdef BYCHARS
    return;
    #endif

    char buffer[BLOCK_SIZE];
    size_t read_bytes;

    fseek(from, 0, SEEK_END);
    long long file_size = ftell(from);

    int no_blocks = file_size / BLOCK_SIZE;
    int remainings = file_size % BLOCK_SIZE;

    if (remainings > 0) {
        char remainings_buffer[remainings];
        fseek(from, -remainings, SEEK_END);
        read_bytes = fread(remainings_buffer, sizeof(char), remainings, from);

        char buffer_to_write[remainings];
        for(int j = 0; j < read_bytes; j++){
            buffer_to_write[j] = remainings_buffer[read_bytes - j - 1];
        }

        fseek(to, 0, SEEK_SET);
        fwrite(buffer_to_write, sizeof(char), read_bytes, to);
    }

    for (int i = no_blocks - 1; i >= 0; i--) {
        fseek(from, i * BLOCK_SIZE, SEEK_SET);
        read_bytes = fread(buffer, sizeof(char), BLOCK_SIZE, from);

        char buffer_to_write[BLOCK_SIZE];
        for(int j = 0; j < read_bytes; j++){
            buffer_to_write[j] = buffer[read_bytes - j - 1];
        }

        fseek(to, (no_blocks - i - 1) * BLOCK_SIZE + remainings, SEEK_SET);
        fwrite(buffer_to_write, sizeof(char), read_bytes, to);
    }
}

int main(int argc, char* argv[]){
    if(argc != 3){
        printf("Wrong arguments\n");
        return EXIT_FAILURE;
    }

    FILE* from = fopen(argv[1], "r");
    if(from == NULL){
        printf("Failed to open file %s\n", argv[1]);
        return EXIT_FAILURE;
    }

    FILE* to = fopen(argv[2], "w");
    if(to == NULL){
        printf("Failed to open file %s\n", argv[2]);
        return EXIT_FAILURE;
    }

    FILE* results = fopen("pomiar_zad_2.txt", "w");
    if(results == NULL){
        printf("Failed to open file pomiar_zad_2.txt\n");
        return EXIT_FAILURE;
    }

    clock_t start, end;
    double time_elapsed;

    #ifndef BYBLOCKS
    start = clock();
    copy_file_by_chars(from, to);
    end = clock();
    time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;
    fprintf(results, "Time taken to copy by characters: %f seconds\n", time_elapsed);
    #endif

    #ifndef BYCHARS
    start = clock();
    copy_file_by_blocks(from, to);
    end = clock();
    time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;
    fprintf(results, "Time taken to copy by blocks:     %f seconds\n", time_elapsed);
    #endif

    fclose(from);
    fclose(to);
    fclose(results);
}