 /*Write a program to create two processes. First process takes a
string and passes it to second process through a pipe. The
second process concatenates the received string with another
string without using string function and sends it back to the first
process for printing.*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#define BUFFER_SIZE 100

void concatenate(char *str1, const char *str2)
{
    while (*str1)
        str1++;
    while (*str2)
        *str1++ = *str2++;
    *str1 = '\0';
}
int main()
{
    int pipefd1[2];
    int pipefd2[2];
    pid_t pid;
    char buffer[BUFFER_SIZE];
    pipe(pipefd1);
    pipe(pipefd2);
    pid = fork();
    if (pid == 0)
    {
        close(pipefd1[1]);
        close(pipefd2[0]);
        read(pipefd1[0], buffer, BUFFER_SIZE);
        close(pipefd1[0]);
        concatenate(buffer, " World");
        write(pipefd2[1], buffer, strlen(buffer) + 1);
        close(pipefd2[1]);
    }
    else
    {
        close(pipefd1[0]);
        close(pipefd2[1]);
        write(pipefd1[1], "Hello", strlen("Hello") + 1);
        close(pipefd1[1]);
        wait(NULL);
        read(pipefd2[0], buffer, BUFFER_SIZE);
        close(pipefd2[0]);
        printf("Final String: %s\n", buffer);
    }
    return 0;
}