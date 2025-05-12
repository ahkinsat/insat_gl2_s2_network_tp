#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>

#define DEFAULT_PORT 80
#define DEFAULT_ADDRESS "example.com"
#define KB 1024
#define REQUEST_SIZE 1*KB
#define RESPONSE_SIZE 4*KB

int main(int argc, char *argv[]) {
    int sock;
    struct sockaddr_in server;
    char request[REQUEST_SIZE + 1], response[RESPONSE_SIZE + 1];
    char buffer[1024];
    
    // Set defaults
    const char *address = DEFAULT_ADDRESS;
    int port = DEFAULT_PORT;
    
    // Parse command line arguments
    if (argc > 1) {
        address = argv[1];
        if (argc > 2) {
            port = atoi(argv[2]);
            if (port <= 0 || port > 65535) {
                fprintf(stderr, "Invalid port number: %d\n", port);
                exit(EXIT_FAILURE);
            }
        }
    }

    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // Set server address
    server.sin_family = AF_INET;
    server.sin_port = htons(port);

    // Resolve domain name to IP address if necessary
    struct hostent* host = gethostbyname(address);
    if (host == NULL) {
        perror("Error resolving domain name");
        close(sock);
        exit(EXIT_FAILURE);
    }
    server.sin_addr.s_addr = *(unsigned long*)host->h_addr;

    // Connect to server
    if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
        perror("Error connecting to server");
        close(sock);
        exit(EXIT_FAILURE);
    }

    // Input HTTP query
    printf("Enter HTTP request (e.g., GET / HTTP/1.1), finish with double newline or EOF (^D):\n");
    request[0] = '\0';
    int newline_count = 0;
    while (1) {
        if (fgets(buffer, 1024, stdin) == NULL) {
            break; // EOF
        }
        if (strlen(request) + strlen(buffer) >= REQUEST_SIZE) {
            printf("Request too large, truncating...\n");
            break;
        }
        strcat(request, buffer);
        if (strcmp(buffer, "\n") == 0) {
            newline_count++;
            if (newline_count == 2) {
                break; // double newline
            }
        } else {
            newline_count = 0;
        }
    }

    // Convert request to CRLF if necessary
    for (size_t i = 0; i < strlen(request); i++) {
        if (request[i] == '\n' && (i == 0 || request[i-1] != '\r')) {
            memmove(request + i + 1, request + i, strlen(request + i) + 1);
            request[i] = '\r';
        }
    }
    //strcat(request, "\r\n");

    // Send request
    ssize_t bytes_sent = send(sock, request, strlen(request), 0);
    if (bytes_sent < 0) {
        perror("Error sending request");
        close(sock);
        exit(EXIT_FAILURE);
    } else if ((size_t)bytes_sent != strlen(request)) {
        fprintf(stderr, "Error sending request: only %zd of %zu bytes sent\n", bytes_sent, strlen(request));
        close(sock);
        exit(EXIT_FAILURE);
    }
    printf("Request sent: '''%s'''", request);

    // Receive response
    ssize_t bytes_received = recv(sock, response, RESPONSE_SIZE, 0);
    if (bytes_received < 0) {
        perror("Error receiving response");
        close(sock);
        exit(EXIT_FAILURE);
    }
    response[bytes_received] = '\0';
    printf("%s", response);

    // Close socket
    if (close(sock) < 0) {
        perror("Error closing socket");
        exit(EXIT_FAILURE);
    }
    return 0;
}