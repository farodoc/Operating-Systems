#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

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

int main() {
    mkfifo("pipe1", 0666);
    mkfifo("pipe2", 0666);

    int fd1 = open("pipe1", O_RDONLY);
    double start, end;
    
    read(fd1, &start, sizeof(double));
    read(fd1, &end, sizeof(double));

    double result = calculate_integral(start, end, 0.01);

    int fd2 = open("pipe2", O_WRONLY);
    write(fd2, &result, sizeof(double));

    close(fd1);
    close(fd2);

    return 0;
}