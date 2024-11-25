// Server.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#define PORT 12347
#define BUFFER_SIZE 1024
int main()
{
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received;
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    if (bind(server_socket, (struct sockaddr *)&server_addr,
             sizeof(server_addr)) < 0)
    {
        perror("Bind failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }
    if (listen(server_socket, 1) < 0)
    {
        perror("Listen failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }
    printf("Server listening on port %d\n", PORT);
    while (1)
    {
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_socket < 0)
        {
            perror("Accept failed");
            continue;
        }
        printf("Accepted connection from %s\n",
               inet_ntoa(client_addr.sin_addr));
        while ((bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0)) > 0)
        {
            buffer[bytes_received] = '\0'; // Null-terminate the received data
            fgets(buffer, sizeof(buffer), stdout);
            send(client_socket, buffer, bytes_received, 0); // Echo the data back
        }
        close(client_socket);
    }
    close(server_socket);
    return 0;
}

// Client.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#define PORT 12347
#define BUFFER_SIZE 1024
int main()
{
    int client_socket;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    ssize_t bytes_sent, bytes_received;
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(PORT);
    if (connect(client_socket, (struct sockaddr *)&server_addr,
                sizeof(server_addr)) < 0)
    {
        perror("Connection failed");
        close(client_socket);
        exit(EXIT_FAILURE);
    }
    while (1)
    {
        printf("Send message: ");
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = '\0'; // Remove newline character
        bytes_sent = send(client_socket, buffer, strlen(buffer), 0);
        if (bytes_sent < 0)
        {
            perror("Send failed");
            break;
        }
        bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1,
                              0);
        if (bytes_received < 0)
        {
            perror("Receive failed");
            break;
        }
        buffer[bytes_received] = '\0'; // Null-terminate the received data
        printf("Received: %s\n", buffer);
    }
    close(client_socket);
    return 0;
}