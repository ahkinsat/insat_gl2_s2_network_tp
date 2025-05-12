#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>
#include <signal.h>
#include <sys/utsname.h>
#include <sys/types.h>  
#include <dirent.h>     
#include <ctype.h>

#define PORT 8080
#define MAX_CLIENTS 10

void handle_time_service(int client_fd) {
    char buf[256];
    for (int i = 0; i < 60; i++) {
        time_t now = time(NULL);
        struct tm *tm = localtime(&now);
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", tm);
        
        if (send(client_fd, buf, strlen(buf), 0) < 0) {
            perror("send failed");
            break;
        }
        sleep(1);
    }
}

int count_all_processes() {
    DIR *dir;
    struct dirent *entry;
    int count = 0;

    if ((dir = opendir("/proc")) == NULL) {
        perror("opendir");
        return -1;
    }

    while ((entry = readdir(dir)) != NULL) {
        // Check if directory name is a PID (numeric)
        int is_pid = 1;
        for (int i = 0; entry->d_name[i] != '\0'; i++) {
            if (!isdigit(entry->d_name[i])) {
                is_pid = 0;
                break;
            }
        }

        if (is_pid) {
            count++;  // Count all process directories
        }
    }

    closedir(dir);
    return count;
}

void handle_nbproc_service(int client_fd) {
    char buf[256];
    int total_procs = count_all_processes();
    
    if (total_procs >= 0) {
        snprintf(buf, sizeof(buf), "Total processes on system: %d", total_procs);
    } else {
        snprintf(buf, sizeof(buf), "Error counting processes");
    }
    
    send(client_fd, buf, strlen(buf), 0);
}

void handle_sysinfo_service(int client_fd) {
    char buf[512];
    struct utsname sys_info;
    
    if (uname(&sys_info) == 0) {
        snprintf(buf, sizeof(buf), 
                "System: %s %s %s\n"
                "Hostname: %s\n"
                "Release: %s\n",
                sys_info.sysname,
                sys_info.machine,
                sys_info.version,
                sys_info.nodename,
                sys_info.release);
        send(client_fd, buf, strlen(buf), 0);
    } else {
        const char *err = "Failed to get system info\n";
        send(client_fd, err, strlen(err), 0);
    }
}

void handle_client(int client_fd) {
    char service_name[32];
    
    // Receive requested service name
    int bytes = recv(client_fd, service_name, sizeof(service_name)-1, 0);
    if (bytes <= 0) {
        perror("recv service name failed");
        close(client_fd);
        return;
    }
    service_name[bytes] = '\0';
    
    printf("Client %d requested service: %s\n", client_fd, service_name);
    
    // Select appropriate service handler
    if (strcmp(service_name, "time") == 0) {
        handle_time_service(client_fd);
    } 
    else if (strcmp(service_name, "nbproc") == 0) {
        handle_nbproc_service(client_fd);
    }
    else if (strcmp(service_name, "sysinfo") == 0) {
        handle_sysinfo_service(client_fd);
    }
    else {
        const char *err = "Unknown service\n";
        send(client_fd, err, strlen(err), 0);
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
        close(client_fd);
    }

    close(server_fd);
    return 0;
}