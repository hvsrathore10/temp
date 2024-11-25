// Server.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#define PORT 8080
#define BUF_SIZE 1024
void reverse_string(char *str)
{
    int len = strlen(str);
    for (int i = 0; i < len / 2; i++)
    {
        char temp = str[i];
        str[i] = str[len - i - 1];
        str[len - i - 1] = temp;
    }
}
int main()
{
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[BUF_SIZE];
    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_DGRAM, 0)) <
        0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    // Configure server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    // Bind the socket
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    printf("Stateless server listening on port %d...\n", PORT);
    while (1)
    {
        // Receive a message from the client
        int bytes_received = recvfrom(server_fd, buffer, BUF_SIZE, 0,
                                      (struct sockaddr *)&client_addr, &client_len);
        if (bytes_received < 0)
        {
            perror("Receive failed");
            continue;
        }
        buffer[bytes_received] = '\0'; // Null-terminate the string
        printf("Received: %s\n", buffer);
        // Process the request (reverse the string)
        reverse_string(buffer);
        // Send the response back to the client
        sendto(server_fd, buffer, strlen(buffer),
               0,
               (struct sockaddr *)&client_addr, client_len);
    }
    close(server_fd);
    return 0;
}
// Client.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#define PORT 8080
#define BUF_SIZE 1024
int main()
{
    int sock;
    struct sockaddr_in server_addr;
    char buffer[BUF_SIZE], message[BUF_SIZE];
    socklen_t addr_len = sizeof(server_addr);
    // Create socket
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) <
        0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    // Configure server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    printf("Type messages to send (type 'exit' to quit):\n");
    while (1)
    {
        printf("You: ");
        fgets(message, BUF_SIZE, stdin);
        message[strcspn(message, "\n")] = '\0'; // Remove trailing newline
        if (strcmp(message, "exit") == 0)
        {
            printf("Exiting...\n");
            break;
        }
        // Send the message to the server
        sendto(sock, message, strlen(message), 0, (struct sockaddr *)&server_addr, addr_len);
        // Receive the response from the server
        int bytes_received = recvfrom(sock, buffer, BUF_SIZE, 0, NULL,
                                      NULL);
        if (bytes_received < 0)
        {
            perror("Receive failed");
            continue;
        }
        buffer[bytes_received] = '\0'; // Null-terminate the
        // string 
        printf("Server: %s\n", buffer);
    }
    close(sock);
    return 0;
}