//Server.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <arpa/inet.h>
#define PORT 8080
#define BUF_SIZE 1024
#define MAX_CLIENTS 10
    void *
    client_handler(void *client_sock)
{
    int sock = *(int *)client_sock;
    char buffer[BUF_SIZE];
    int bytes_read;
    while ((bytes_read = recv(sock, buffer, BUF_SIZE, 0)) > 0)
    {
        buffer[bytes_read] = '\0';
        printf("Received from client: %s\n", buffer);
        send(sock, buffer, strlen(buffer), 0); // Echo back the message
    }
    close(sock);
    free(client_sock);
    printf("Client disconnected.\n");
    return NULL;
}
int main()
{
    int server_fd, client_fd, *client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) ==
        0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr =
        INADDR_ANY;
    server_addr.sin_port =
        htons(PORT);
    // Bind the socket
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    // Listen for incoming connections
    if (listen(server_fd, MAX_CLIENTS) < 0)
    {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    printf("Server listening on port %d...\n", PORT);
    while (1)
    {
        client_fd = accept(server_fd, (struct sockaddr *)&client_addr,
                           &client_len);
        if (client_fd < 0)
        {
            perror("Accept failed");
            continue;
        }
        printf("Client connected.\n");
        client_sock = malloc(sizeof(int));
        *client_sock = client_fd;
        pthread_t thread_id;
        if (pthread_create(&thread_id, NULL, client_handler, (void *)client_sock) != 0)
        {
            perror("Thread creation failed");
            close(client_fd);
            free(client_sock);
        }
        pthread_detach(thread_id); // Automatically reclaim resources when thread exits
    }
    close(server_fd);
    return 0;
}
//Client.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#define PORT 8080
#define BUF_SIZE 1024
    int
    main()
{
    int sock;
    struct sockaddr_in server_addr;
    char buffer[BUF_SIZE], message[BUF_SIZE];
    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) <
        0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr =
        inet_addr("127.0.0.1");
    // Connect to server
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Connection failed");
        close(sock);
        exit(EXIT_FAILURE);
    }
    printf("Connected to server. Type messages to send:\n");
    while (1)
    {
        printf("You: ");
        fgets(message, BUF_SIZE, stdin);
        message[strcspn(message, "\n")] = '\0'; // Remove trailing newline
        if (strcmp(message, "exit") == 0)
        {
            printf("Disconnecting...\n");
            break;
        }
        send(sock, message, strlen(message), 0);
        int bytes_read = recv(sock, buffer,
                              BUF_SIZE, 0);
        if (bytes_read > 0)
        {
            buffer[bytes_read] = '\0';
            printf("Server: %s\n", buffer);
        }
    }
    close(sock);
    return 0;
}