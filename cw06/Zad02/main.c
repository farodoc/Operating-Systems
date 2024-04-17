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

    int fd1 = open("pipe1", O_WRONLY);

    write(fd1, &start, sizeof(double));
    write(fd1, &end, sizeof(double));

    int fd2 = open("pipe2", O_RDONLY);
    double result;
    
    read(fd2, &result, sizeof(double));

    printf("The result is: %lf\n", result);

    close(fd1);
    close(fd2);

    return 0;
}