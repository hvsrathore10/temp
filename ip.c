// server.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#define PORT 12349
#define MAX_CLIENTS 100
#define BUFFER_SIZE 1024
typedef struct
{
    char node[20];
    char ip[INET_ADDRSTRLEN];
    int port;
} TableEntry;
TableEntry server_table[MAX_CLIENTS];
int table_size = 0;
void update_table(const char *ip, int port)
{
    // Check if the IP and port are already in the table
    int i;
    for (i = 0; i < table_size; i++)
    {
        if (strcmp(server_table[i].ip, ip) == 0 && server_table[i].port ==
                                                       port)
        {
            return;
        }
    }
    // Add new entry to the table
    snprintf(server_table[table_size].node,
             sizeof(server_table[table_size].node), "Client-%d", table_size + 1);
    strncpy(server_table[table_size].ip, ip,
            sizeof(server_table[table_size].ip));
    server_table[table_size].port = port;
    table_size++;
}
void print_table()
{
    printf("Master Table:\n");
    int i;
    for (i = 0; i < table_size; i++)
    {
        printf("Node: %s, IP: %s, Port: %d\n", server_table[i].node,
               server_table[i].ip, server_table[i].port);
    }
    printf("\n");
}
void send_table(int client_socket)
{
    write(client_socket, &table_size, sizeof(int)); // Send table size
    write(client_socket, server_table, sizeof(TableEntry) * table_size);
    // Send table entries
}
int main()
{
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
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
    if (listen(server_socket, 5) < 0)
    {
        perror("Listen failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }
    printf("Server is listening on port %d\n", PORT);
    while (1)
    {
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_socket < 0)
        {
            perror("Accept failed");
            continue;
        }
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip,
                  sizeof(client_ip));
        int client_port = ntohs(client_addr.sin_port);
        update_table(client_ip, client_port);
        // Print the updated master table
        print_table();
        send_table(client_socket);
        close(client_socket);
    }
    close(server_socket);
    return 0;
}



// client.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#define PORT 12349
#define BUFFER_SIZE 1024
    typedef struct
{
    char node[20];
    char ip[INET_ADDRSTRLEN];
    int port;
} TableEntry;
void receive_table(int server_socket)
{
    int table_size;
    read(server_socket, &table_size, sizeof(int)); // Read table size
    TableEntry *table = (TableEntry *)malloc(sizeof(TableEntry) *
                                             table_size);
    if (table == NULL)
    {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }
    read(server_socket, table, sizeof(TableEntry) * table_size); // Read table entries
        printf("Received table:\n");
    int i;
    for (i = 0; i < table_size; i++)
    {
        printf("Node: %s, IP: %s, Port: %d\n", table[i].node, table[i].ip,
               table[i].port);
    }
    free(table);
}
int main()
{
    int client_socket;
    struct sockaddr_in server_addr;
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    if (connect(client_socket, (struct sockaddr *)&server_addr,
                sizeof(server_addr)) < 0)
    {
        perror("Connect failed");
        close(client_socket);
        exit(EXIT_FAILURE);
    }
    receive_table(client_socket);
    close(client_socket);
    return 0;
}