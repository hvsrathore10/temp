/*Write a program in which the parent process sends two matrices
to its child process through a pipe and the child process returns
the sum of the matrices to the parent through a pipe. The parent
should print the result.*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
int main()
{
    int pipefd1[2], pipefd2[2];
    pid_t pid;
    int rows, cols;
    printf("Enter number of rows -> ");
    scanf("%d", &rows);
    printf("Enter number of columns -> ");
    scanf("%d", &cols);
    int mat1[rows][cols];
    int mat2[rows][cols];
    int result[rows][cols];
    printf("Enter elements of first matrix: -> \n");
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            scanf("%d", &mat1[i][j]);
    printf("Enter elements of second matrix: -> \n");
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            scanf("%d", &mat2[i][j]);
    pipe(pipefd1);
    pipe(pipefd2);
    pid = fork();
    if (pid == 0)
    {
        close(pipefd1[1]);
        close(pipefd2[0]);
        for (int i = 0; i < rows; i++)
        {
            read(pipefd1[0], mat1[i], cols * sizeof(int));
            read(pipefd1[0], mat2[i], cols * sizeof(int));
        }
        close(pipefd1[0]);
        for (int i = 0; i < rows; i++)
            for (int j = 0; j < cols; j++)
                result[i][j] = mat1[i][j] + mat2[i][j];
        for (int i = 0; i < rows; i++)
        {
            write(pipefd2[1], result[i], cols * sizeof(int));
        }
        close(pipefd2[1]);
    }
    else
    {
        close(pipefd1[0]);
        close(pipefd2[1]);
        for (int i = 0; i < rows; i++)
        {
            write(pipefd1[1], mat1[i], cols * sizeof(int));
            write(pipefd1[1], mat2[i], cols * sizeof(int));
        }
        close(pipefd1[1]);
        wait(NULL);
        for (int i = 0; i < rows; i++)
        {
            read(pipefd2[0], result[i], cols * sizeof(int));
        }
        close(pipefd2[0]);
        printf("Sum of matrices:\n");
        for (int i = 0; i < rows; i++)
        {
            for (int j = 0; j < cols; j++)
            {
                printf("%d ", result[i][j]);
            }
            printf("\n");
        }
    }
    return 0;
}