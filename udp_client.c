/*Write a client (UDP) C program that calls sendto() to send string
to server program knowing IP address and port number.
*/// udp_client.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 12345
#define BUFFER_SIZE 1024

int main()
{
    int sockfd;
    // Replace with the server's port
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    ssize_t bytes_sent;
    socklen_t addr_len = sizeof(server_addr);
    // Create a UDP socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    // Specify the server address and port
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    // Convert IP address from text to binary form
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0)
    {
        perror("inet_pton");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    // Prepare the message to send
    snprintf(buffer, sizeof(buffer), "Hello, Server!");
    // Send the message to the server
    bytes_sent = sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&server_addr, addr_len);
    if (bytes_sent < 0)
    {
        perror("sendto");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    printf("Sent %zd bytes to %s:%d\n", bytes_sent, SERVER_IP,
           SERVER_PORT);
    // Close the socket
    close(sockfd);
    return 0;
}