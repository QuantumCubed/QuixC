#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stdatomic.h>
#include "http.h"

// void create_http_server() {

// }

// void destroy_http_server() {

// }

volatile sig_atomic_t SHUTDOWN_REQ = 0;
const char IP[] = "127.0.0.1";
const uint16_t PORT = 3000;

enum SERVER_CODES {
    SERVER_OK = 0,
    SERVER_ERR_CLOSE = 1,
    SERVER_ERR_BIND  = 2,
    SERVER_ERR_LISTEN = 3,
    SERVER_ERR_ACCEPT = 4,
};

void SIGNAL_HANDLER(int sig) {
    printf("\nSIG: %d\n", sig);
    SHUTDOWN_REQ = 1;
}

int SERVER_SOCKET_INIT() {
    struct sockaddr_in address;
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (server_socket == -1)
    {
        exit(EXIT_FAILURE); // CHANGE THIS
    }

    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);
    address.sin_addr.s_addr = inet_addr(IP);

    bind(server_socket, (struct sockaddr *)&address, sizeof(address));

    listen(server_socket, 3);

    printf("The Server Is Listening On:\nIP: %s\nPORT: %u\n", IP, PORT);

    return server_socket;
}

int SERVER_SOCKET_TERMINATE(int server_socket_fd) {
    printf("SHUTTING SERVER DOWN...\n");

    if(close(server_socket_fd) == -1) {
        int error = errno;
        fprintf(stderr, "GRACEFUL SHUTDOWN UNSUCCESSFUL!\n");
        perror("socket_shutdown");
        return SERVER_ERR_CLOSE;
    }

    printf("GRACEFUL SHUTDOWN SUCCESSFUL!\n");
    return 0;
}

/// *** ///

// CLIENT MESSAGE: GET / HTTP/1.1
// Accept: application/json, text/plain, */*
// Content-Type: text/plain
// User-Agent: bruno-runtime/2.9.1
// request-start-time: 1764782766231
// Content-Length: 4
// Accept-Encoding: gzip, compress, deflate, br
// Host: 127.0.0.1:3000
// Connection: keep-alive

// test

/// *** ///

void substring(const char *src, char *dest, int startIndex, int endIndex) {
    int i = 0;
    for(; startIndex < endIndex; ++startIndex) {
        // printf("%c\n", src[startIndex]);
        dest[i] = src[startIndex];
        ++i;
    }
    dest[i] = '\0';
}


void req_handler(char *buffer) {
    // printf("CLIENT MESSAGE: %s\n", buffer);
    // printf("Start Line: ")
    // int i;
    // int newStartIndex = 0;
    // for(i = 0; i < BUFFER_SIZE; ++i) {
    //     if(buffer[i] == '\0') {
    //         return;
    //     }
    //     if(buffer[i] == '\n') {
    //         char line[i + 1];
    //         substring(buffer, line, newStartIndex, i);
    //         printf("LINE: %s\n", line);
    //         newStartIndex = i + 1;
    //     }
    // }
    char *saveptr;
    char *line = strtok_r(buffer, "\n", &saveptr);
    while(line != NULL) {
        // printf("LINE: %s\n", line);
        printf("%s\n", line);
        line = strtok_r(NULL, "\n", &saveptr);
    }
}

int main()
{

    struct sigaction SIGACTION_ARGS;
    SIGACTION_ARGS.sa_handler = SIGNAL_HANDLER;
    sigemptyset(&SIGACTION_ARGS.sa_mask);
    SIGACTION_ARGS.sa_flags = 0;   // IMPORTANT: no SA_RESTART

    sigaction(SIGINT, &SIGACTION_ARGS, NULL);

    int server_socket = SERVER_SOCKET_INIT();

    while(!SHUTDOWN_REQ)
    {
        int client_socket = accept(server_socket, NULL, NULL);

        char buffer[BUFFER_SIZE];

        recv(client_socket, buffer, BUFFER_SIZE - 1, 0);

        req_handler(buffer);

        char message[] = "Request Recieved!\n";

        send(client_socket, message, strlen(message), 0);

        close(client_socket);
    }

    int cleanup_status = SERVER_SOCKET_TERMINATE(server_socket);

    return (cleanup_status == SERVER_OK) ? EXIT_SUCCESS : EXIT_FAILURE;
}