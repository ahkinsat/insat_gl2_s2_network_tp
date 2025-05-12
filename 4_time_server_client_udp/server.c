#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <time.h>

#define SERVER_PORT 8080
#define BUFFER_SIZE 256

int main() {
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];

    // Create UDP socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket
    if (bind(sockfd, (const struct sockaddr *)&server_addr, 
            sizeof(server_addr)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    printf("Server waiting for messages...\n");

    // Receive initial message
    int n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, 
                    (struct sockaddr *)&client_addr, &client_len);
    if (n < 0) {
        perror("Receive failed");
        exit(EXIT_FAILURE);
    }
    buffer[n] = '\0';
    printf("Received message: %s\n", buffer);

    // Send time 60 times
    for (int i = 0; i < 60; i++) {
        time_t current_time = time(NULL);
        struct tm *time_info = localtime(&current_time);
        strftime(buffer, BUFFER_SIZE, "%Y-%m-%d %H:%M:%S", time_info);

        if (sendto(sockfd, buffer, strlen(buffer), 0, 
                  (const struct sockaddr *)&client_addr, client_len) < 0) {
            perror("Send failed");
            break;
        }
        printf("Sent time: %s\n", buffer);
        sleep(1);
    }

    close(sockfd);
    return 0;
}