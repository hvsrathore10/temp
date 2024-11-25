//nodeB.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#define PORT 8080

void get_cpu_load(char *buffer) {
    FILE *fp = popen("sysctl -n vm.loadavg", "r");
    if (fp == NULL) {
        perror("popen");
        exit(EXIT_FAILURE);
    }
    fgets(buffer, 256, fp);
    pclose(fp);
}

void format_cpu_load(const char *loadavg, char *formatted) {
    float one_min, five_min, fifteen_min;
    int read_count = sscanf(loadavg, "{ %f %f %f }", &one_min, &five_min, &fifteen_min);
    if (read_count == 3) {
        snprintf(formatted, 256, "Load Average:\n1 minute: %.2f\n5 minutes: %.2f\n15 minutes: %.2f",
                 one_min, five_min, fifteen_min);
    } else {
        snprintf(formatted, 256, "Error: Could not parse load average.");
    }
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[256] = {0};
    char formatted_buffer[256] = {0};

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Binding socket to port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    get_cpu_load(buffer);
    format_cpu_load(buffer, formatted_buffer);
    send(new_socket, formatted_buffer, strlen(formatted_buffer), 0);
    printf("Formatted CPU load sent: %s\n", formatted_buffer);

    close(new_socket);
    close(server_fd);
    return 0;
}
