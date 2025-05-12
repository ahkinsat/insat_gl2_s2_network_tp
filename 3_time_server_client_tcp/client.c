#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8080

int main() {
    int client_socket;
    struct sockaddr_in server_addr;
    char buffer[256];

    // Create the socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Socket creation failed");
        exit(1);
    }

    // Set up server address
    memset(&server_addr, 0, sizeof(struct sockaddr_in));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    // Connect to the server
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_in)) == -1) {
        perror("Connect failed");
        exit(1);
    }

    // Send a message to the server (e.g., "Hello")
    strcpy(buffer, "Hello");
    send(client_socket, buffer, strlen(buffer), 0);
    printf("Sent message: %s\n", buffer);

    // Receive 60 time responses from the server
    for (int i = 0; i < 60; i++) {
        int n = recv(client_socket, buffer, sizeof(buffer), 0);
        if (n <= 0) {
            printf("Failed to receive time from server\n");
            break;
        }
        buffer[n] = '\0';
        printf("Received time: %s\n", buffer);
    }

    // Close the socket
    close(client_socket);
    return 0;
}
