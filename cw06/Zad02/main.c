#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

int main() {
    double start, end;

    printf("Enter the start of the interval: ");
    scanf("%lf", &start);

    printf("Enter the end of the interval: ");
    scanf("%lf", &end);

    int fd = open("pipe", O_WRONLY);

    write(fd, &start, sizeof(double));
    write(fd, &end, sizeof(double));
    close(fd);

    fd = open("pipe", O_RDONLY);
    double result;
    
    read(fd, &result, sizeof(double));
    close(fd);

    printf("The result is: %lf\n", result);

    return 0;
}