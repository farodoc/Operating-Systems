#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>

double min(double a, double b){
    return a < b ? a : b;
}

double f(double x){
    return 4/(x*x+1);
}

double calculate_integral(double start, double end, double rect_width){
    double res = 0;

    double a = start;
    double b = start + rect_width;

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

    double rect_width = atof(argv[1]);
    int no_processes = atoi(argv[2]);

    if(ceil(1/rect_width) < no_processes){
        printf("Too many processes\n");
        return -1;
    }

    double res = 0;

    struct timeval start_time, end_time;
    gettimeofday(&start_time, NULL);

    int fd[no_processes][2];

    unsigned long long no_intervals_total = (unsigned long long)ceil(1/rect_width);
    unsigned long long no_intervals_per_process = no_intervals_total/no_processes;
    double interval_width = min(no_intervals_per_process * rect_width, 1);

    double start = 0;
    double end = interval_width;

    for(int i = 0; i < no_processes; i++){
        if(pipe(fd[i]) < 0){
            printf("Failed to create pipe\n");
            return -1;
        }

        pid_t pid = fork();

        if(pid == 0){
            close(fd[i][0]);

            double temp_res = calculate_integral(start, end, rect_width);

            if(write(fd[i][1], &temp_res, sizeof(double)) < 0){
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
        double temp_res;
        if(read(fd[i][0], &temp_res, sizeof(double)) < 0){
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
    printf("Rect width: %lf\n", rect_width);
    printf("Processes: %d\n\n", no_processes);
    printf("Result: %lf\n\n", res);
    printf("Time: %f seconds\n", time_elapsed);
    printf("--------------------------\n");

    FILE *file = fopen("results.txt", "w");
    if (file == NULL) {
        printf("Failed to open file\n");
        return -1;
    }

    fprintf(file, "Rect width: %lf\n", rect_width);
    fprintf(file, "Processes: %d\n\n", no_processes);
    fprintf(file, "Result: %lf\n\n", res);
    fprintf(file, "Time: %f seconds\n", time_elapsed);

    fclose(file);

    return 0;
}