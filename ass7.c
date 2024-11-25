//  TCP Server Code (server1.c and server2.c)
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <ctype.h>
#define PORT 12345 // Use a different port for server2 (e.g., 12346)
#define BUFFER_SIZE 1024
// Function to calculate CPU usage (dummy value for simplicity)
float get_cpu_load()
{
    return (rand() % 100) / 100.0; // Return a random CPU usage between 0 and 1
}
// Function to convert a string to uppercase
void to_uppercase(char *str)
{
    for (int i = 0; str[i]; i++)
    {
        str[i] = toupper(str[i]);
    }
}
int main()
{
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    char buffer[BUFFER_SIZE];
    socklen_t addr_len;
    // Create socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    // Bind the socket to an address
    bind(server_socket, (struct sockaddr *)&server_addr,
         sizeof(server_addr));
    // Start listening for connections
    listen(server_socket, 5);
    printf("Server listening on port %d...\n", PORT);
    while (1)
    {
        addr_len = sizeof(client_addr);
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_len);
        printf("Accepted connection from load balancer...\n");
        // Receive command (either "CPU" for load or string for conversion)
        recv(client_socket, buffer, BUFFER_SIZE, 0);
        buffer[strlen(buffer)] = '\0'; // Ensure null termination
        if (strcmp(buffer, "CPU") == 0)
        {
            // Send CPU load
            float cpu_load = get_cpu_load();
            send(client_socket, &cpu_load,
                 sizeof(cpu_load), 0);
            printf("Sent CPU load: %.2f\n", cpu_load);
        }
        else
        {
            // Convert string to uppercase
            to_uppercase(buffer);
            send(client_socket, buffer, strlen(buffer),
                 0);
            printf("Sent uppercase string: %s\n",
                   buffer);
        }
        close(client_socket);
    }
    close(server_socket);
    return 0;
}

// Load Balancer Code (broker.c)
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#define SERVER1_PORT 12345
#define SERVER2_PORT 12346
#define BUFFER_SIZE 1024
// Function to query CPU load from a server
float get_cpu_load(int port)
{
    int sock;
    struct sockaddr_in server_addr;
    float cpu_load;
    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    // Connect to the server
    connect(sock, (struct sockaddr *)&server_addr,
            sizeof(server_addr));
    // Send "CPU" request to get CPU load
    send(sock, "CPU", 3, 0);
    recv(sock, &cpu_load, sizeof(cpu_load), 0);
    close(sock);
    return cpu_load;
}
// Function to send a string to a server for conversion
void send_string_to_server(int port, char *str, char *result)
{
    int sock;
    struct sockaddr_in server_addr;
    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    // Connect to the server
    connect(sock, (struct sockaddr *)&server_addr,
            sizeof(server_addr));
    // Send the string for conversion
    send(sock, str, strlen(str), 0);
    recv(sock, result, BUFFER_SIZE, 0);
    close(sock);
}
int main()
{
    int broker_socket, client_socket;
    struct sockaddr_in broker_addr, client_addr;
    char buffer[BUFFER_SIZE], result[BUFFER_SIZE];
    socklen_t addr_len;
    // Create socket for load balancer
    broker_socket = socket(AF_INET, SOCK_STREAM, 0);
    broker_addr.sin_family = AF_INET;
    broker_addr.sin_port = htons(12347); // Broker server port
    broker_addr.sin_addr.s_addr = INADDR_ANY;
    // Bind the socket
    bind(broker_socket, (struct sockaddr *)&broker_addr,
         sizeof(broker_addr));
    listen(broker_socket, 5);
    printf("Load Balancer listening on port 12347...\n");
    while (1)
    {
        addr_len = sizeof(client_addr);
        client_socket = accept(broker_socket, (struct sockaddr *)&client_addr, &addr_len);
        printf("Accepted connection from client...\n");
        // Receive string from the client
        recv(client_socket, buffer, BUFFER_SIZE, 0);
        buffer[strlen(buffer)] = '\0'; // Ensure null termination
        // Get CPU loads from both servers
        float cpu1 = get_cpu_load(SERVER1_PORT);
        float cpu2 = get_cpu_load(SERVER2_PORT);
        printf("CPU load - Server 1: %.2f, Server 2:% .2f\n ", cpu1, cpu2);
        // Send the string to the server with the lower CPU load
        if (cpu1 < cpu2)
        {
            send_string_to_server(SERVER1_PORT, buffer,
                                  result);
        }
        else
        {
            send_string_to_server(SERVER2_PORT, buffer,
                                  result);
        }
        // Send the result back to the client
        send(client_socket, result, strlen(result), 0);
        printf("Sent result back to client: %s\n",
               result);
        close(client_socket);
    }
    close(broker_socket);
    return 0;
}

// client.c
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#define BROKER_PORT 12347
#define BUFFER_SIZE 1024
int main()
{
    int sock;
    struct sockaddr_in broker_addr;
    char buffer[BUFFER_SIZE], result[BUFFER_SIZE];
    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    broker_addr.sin_family = AF_INET;
    broker_addr.sin_port = htons(BROKER_PORT);
    broker_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    // Connect to load balancer
    connect(sock, (struct sockaddr *)&broker_addr,
            sizeof(broker_addr));
    // Input string to send
    printf("Enter a string to convert: ");
    fgets(buffer, BUFFER_SIZE, stdin);
    buffer[strlen(buffer) - 1] = '\0'; // Remove newline
    // Send string to load balancer
    send(sock, buffer, strlen(buffer), 0);
    // Receive the uppercase string
    recv(sock, result, BUFFER_SIZE, 0);
    printf("Received uppercase string: %s\n", result);
    close(sock);
    return 0;
}