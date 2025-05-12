#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>

#define SERVER_PORT 8080

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len;
    char buffer[256];

    // Create the socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Socket creation failed");
        exit(1);
    }

    // Set up server address
    memset(&server_addr, 0, sizeof(struct sockaddr_in));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket to the address
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_in)) == -1) {
        perror("Bind failed");
        exit(1);
    }

    // Listen for connections
    if (listen(server_socket, 1) == -1) {
        perror("Listen failed");
        exit(1);
    }

    printf("Server waiting for connection...\n");

    // Accept incoming connection
    client_len = sizeof(client_addr);
    client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
    if (client_socket == -1) {
        perror("Accept failed");
        exit(1);
    }

    // Receive the client's message (like "Hello")
    int n = recv(client_socket, buffer, sizeof(buffer), 0);
    if (n <= 0) {
        printf("Failed to receive message from client\n");
        close(client_socket);
        close(server_socket);
        return 1;
    }
    buffer[n] = '\0';
    printf("Received message: %s\n", buffer);

    // Send the current time 60 times
    for (int i = 0; i < 60; i++) {
        time_t current_time = time(NULL);
        struct tm *time_info = localtime(&current_time);

        // Format the time into a string
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", time_info);
        send(client_socket, buffer, strlen(buffer), 0);
        printf("Sent time: %s\n", buffer);

        // Wait 1 second before sending the next time
        sleep(1);
    }

    // Close sockets
    close(client_socket);
    close(server_socket);
    return 0;
}
