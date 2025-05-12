#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8080
#define BUFFER_SIZE 256

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    socklen_t addr_len = sizeof(server_addr);

    // Create UDP socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    // Send initial message
    strcpy(buffer, "Hello");
    if (sendto(sockfd, buffer, strlen(buffer), 0, 
              (const struct sockaddr *)&server_addr, addr_len) < 0) {
        perror("Send failed");
        exit(EXIT_FAILURE);
    }
    printf("Sent message: %s\n", buffer);

    // Receive 60 time responses
    for (int i = 0; i < 60; i++) {
        int n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, 
                        (struct sockaddr *)&server_addr, &addr_len);
        if (n < 0) {
            perror("Receive failed");
            break;
        }
        buffer[n] = '\0';
        printf("Received time: %s\n", buffer);
    }

    close(sockfd);
    return 0;
}