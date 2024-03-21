#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>

int main() {
    DIR* dir = opendir(".");

    if (dir == NULL){
        printf("Error opening directory");
        return EXIT_FAILURE;
    }

    struct dirent* entry = readdir(dir);;
    struct stat file_stat;
    long long sum_size = 0;

    while (entry != NULL){
        stat(entry->d_name, &file_stat);

        if (S_ISDIR(file_stat.st_mode)){
            entry = readdir(dir);
            continue;
        }

        printf("%s - %lld bytes\n", entry->d_name, (long long int)file_stat.st_size);
        sum_size += file_stat.st_size;

        entry = readdir(dir);
    }

    printf("\nTotal size: %lld bytes\n", sum_size);
    closedir(dir);

    return EXIT_SUCCESS;
}