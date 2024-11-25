/*program to determine CPU load of node B from node A*/
//nodeA.c 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#define PORT 8080

int main()
{
    struct sockaddr_in serv_addr;
    int sock = 0;
    char buffer[256] = {0};
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Socket creation error");
        return -1;
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
        perror("Invalid address/ Address not supported");
        return -1;
    }
    if (connect(sock, (struct sockaddr *)&serv_addr,
                sizeof(serv_addr)) < 0)
    {
        perror("Connection failed");
        return -1;
    }
    read(sock, buffer, 256);
    printf("CPU load received: %s\n", buffer);
    close(sock);
    return 0;
}