#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>

long double min(long double a, long double b){
    return a < b ? a : b;
}

long double f(long double x){
    return 4/(x*x+1);
}

long double calculate_integral(long double start, long double end, long double rect_width){
    long double res = 0;

    long double a = start;
    long double b = start + rect_width;

    while(b < end){
        res += (f((a + b)/2) * (b - a));
        a = b;
        b += rect_width;
    }

    b = end;
    res += (f((a + b)/2) * (b - a));

    return res;
}

int main(int argc, char *argv[]){
    if(argc < 3){
        printf("Usage: <witdth> <number of processes>\n");
        return -1;
    }

    long double rect_width = strtold(argv[1], NULL);
    int no_processes = atoi(argv[2]);

    if(ceil(1/rect_width) < no_processes){
        printf("Too many processes\n");
        return -1;
    }

    long double res = 0;

    struct timeval start_time, end_time;
    gettimeofday(&start_time, NULL);

    int fd[no_processes][2];

    unsigned long long no_intervals_total = (unsigned long long)ceil(1/rect_width);
    unsigned long long no_intervals_per_process = no_intervals_total/no_processes;
    long double interval_width = min(no_intervals_per_process * rect_width, 1);

    long double start = 0;
    long double end = interval_width;

    for(int i = 0; i < no_processes; i++){
        if(pipe(fd[i]) < 0){
            printf("Failed to create pipe\n");
            return -1;
        }

        pid_t pid = fork();

        if(pid == 0){
            close(fd[i][0]);

            long double temp_res = calculate_integral(start, end, rect_width);

            if(write(fd[i][1], &temp_res, sizeof(long double)) < 0){
                printf("Failed to write to pipe\n");
                return -1;
            }

            return 0;
        }

        close(fd[i][1]);

        start = end;
        end += interval_width;
        
        if(i == no_processes - 2) end = 1;
    }

    for(int i = 0; i < no_processes; i++){
        long double temp_res;
        if(read(fd[i][0], &temp_res, sizeof(long double)) < 0){
            printf("Failed to read from pipe\n");
            return -1;
        }

        res += temp_res;
    }

    gettimeofday(&end_time, NULL);

    long time_elapsed_sec = end_time.tv_sec - start_time.tv_sec;
    long time_elapsed_usec = end_time.tv_usec - start_time.tv_usec;

    double time_elapsed = time_elapsed_sec + time_elapsed_usec / 1000000.0;

    printf("--------------------------\n");
    printf("Rect width: %.15Lf\n", rect_width);
    printf("Processes: %d\n\n", no_processes);
    printf("Result: %Lf\n\n", res);
    printf("Time: %f seconds\n", time_elapsed);
    printf("--------------------------\n");

    FILE *file = fopen("results.txt", "w");
    if (file == NULL) {
        printf("Failed to open file\n");
        return -1;
    }

    fprintf(file, "Rect width: %.15Lf\n", rect_width);
    fprintf(file, "Processes: %d\n\n", no_processes);
    fprintf(file, "Result: %Lf\n\n", res);
    fprintf(file, "Time: %f seconds\n", time_elapsed);

    fclose(file);

    return 0;
}