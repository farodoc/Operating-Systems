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
    mkfifo("pipe", 0666);

    int fd = open("pipe", O_RDONLY);
    double start, end;
    
    read(fd, &start, sizeof(double));
    read(fd, &end, sizeof(double));
    close(fd);

    double result = calculate_integral(start, end, 0.01);

    fd = open("pipe", O_WRONLY);
    write(fd, &result, sizeof(double));
    close(fd);

    return 0;
}