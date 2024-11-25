// Server.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#define PORT 8080
#define BUFFER_SIZE 1024
void send_file(FILE *file, int sockfd)
{
    char data[BUFFER_SIZE] = {0};
    while (fgets(data, BUFFER_SIZE, file) != NULL)
    {
        if (send(sockfd, data, sizeof(data), 0) == -1)
        {
            perror("Error sending file.");
            exit(EXIT_FAILURE);
        }
        memset(data, 0, BUFFER_SIZE);
    }
}
int main()
{
    int sockfd, new_sock;
    struct sockaddr_in server_addr, new_addr;
    socklen_t addr_size;
    char *filename = "test_file.txt";
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        perror("File not found.");
        exit(EXIT_FAILURE);
    }
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("Error in socket.");
        exit(EXIT_FAILURE);
    }
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    bind(sockfd, (struct sockaddr *)&server_addr,
         sizeof(server_addr));
    listen(sockfd, 10);
    addr_size = sizeof(new_addr);
    new_sock = accept(sockfd, (struct sockaddr *)&new_addr, &addr_size);
    send_file(file, new_sock);
    printf("File data sent successfully.\n");
    close(new_sock);
    close(sockfd);
    return 0;
}

// Client.c :
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#define PORT 8080
#define BUFFER_SIZE 1024
int main()
{
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE] = {0};
    FILE *received_file;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("Error in socket.");
        exit(EXIT_FAILURE);
    }
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    if (connect(sockfd, (struct sockaddr *)&server_addr,
                sizeof(server_addr)) == -1)
    {
        perror("Error in connecting.");
        exit(EXIT_FAILURE);
    }
    received_file = fopen("received_test_file.txt", "w");
    if (!received_file)
    {
        perror("Error in creating file.");
        exit(EXIT_FAILURE);
    }
    ssize_t n;
    while ((n = recv(sockfd, buffer, BUFFER_SIZE, 0)) >
           0)
    {
        fprintf(received_file,
                "%s", buffer);
        memset(buffer, 0, BUFFER_SIZE);
    }
    printf("File received successfully.\n");
    fclose(received_file);
    close(sockfd);
    return 0;
}