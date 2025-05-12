#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <time.h>
#include <signal.h>

#define PORT 8080
#define MAX_CLIENTS 10
/* // ---
void sleep_ms(int milliseconds) {
    struct timeval tv;
    tv.tv_sec = milliseconds / 1000; // Whole seconds
    tv.tv_usec = (milliseconds % 1000) * 1000; // Remaining microseconds

    // Use select with no file descriptors to sleep
    select(0, NULL, NULL, NULL, &tv);
}
// --- */

void handle_client(int client_fd) {
    char buf[256];
    
    // 1. Receive client's "Hello" message
    if (recv(client_fd, buf, sizeof(buf), 0) <= 0) {
        perror("recv hello failed");
        close(client_fd);
        return;
    }
    printf("Client %d said: %s\n", client_fd, buf);

    // 2. Send 60 time responses (1 per second)
    for (int i = 0; i < 60; i++) {
        time_t now = time(NULL);
        struct tm *tm = localtime(&now);
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", tm);
        
        if (send(client_fd, buf, strlen(buf), 0) < 0) {
            perror("send failed");
            break;
        }
        printf("Sent to %d: %s\n", client_fd, buf);
        sleep(1);
        //sleep_ms(1000);
    }
    
    close(client_fd);
}

int main() {
    int server_fd, client_fd;
    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);

    // Create server socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        exit(1);
    }

    // Configure server address
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);

    // Bind and listen
    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind failed");
        exit(1);
    }

    if (listen(server_fd, MAX_CLIENTS) < 0) {
        perror("listen failed");
        exit(1);
    }

    printf("Server ready on port %d\n", PORT);
    signal(SIGCHLD, SIG_IGN);  // Auto-reap child processes

    while (1) {
        // Accept new client
        if ((client_fd = accept(server_fd, (struct sockaddr*)&addr, &addrlen)) < 0) {
            perror("accept failed");
            continue;
        }

        printf("New client: %s:%d (fd=%d)\n", 
               inet_ntoa(addr.sin_addr), ntohs(addr.sin_port), client_fd);

        // Fork to handle client
        if (fork() == 0) {  // Child process
            close(server_fd);
            handle_client(client_fd);
            exit(0);
        }
        close(client_fd);  // Parent doesn't need client_fd
    }

    close(server_fd);
    return 0;
}