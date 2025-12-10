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
#include <ctype.h>

// void create_http_server() {

// }

// void destroy_http_server() {

// }

volatile sig_atomic_t SHUTDOWN_REQ = 0;
const char IP[] = "0.0.0.0";
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

void strip_space(char *src, uint32_t line_len) {
    uint32_t p1 = 0;
    uint32_t p2 = 0;
    
    for(; p1 < line_len; ++p1) {
	    if(!isspace(src[p1])) {
		    src[p2] = src[p1];
		    ++p2;
	    }
    }
    
    src[p2] = '\0';
}

void req_handler(char *buffer) {
    char *saveptr_line;
    char *line = strtok_r(buffer, "\n", &saveptr_line);
    bool first_line = true;

    while(line != NULL) {
	strip_space(line, strlen(line));
	if(first_line) {
		//char *saveptr_sub_token;
		//char *method = strtok_r(line, " ", &saveptr_sub_token);
	    	//char *path = strtok_r(NULL, " ", &saveptr_sub_token);
		//char *version = strtok_r(NULL, " ", &saveptr_sub_token);
	   	
		//strip_space(line, strlen(line));

		//printf("%s%s%s\n", method, path, version);
		first_line = false;
		continue;
	} else {
		char *saveptr_sub_token;

		char *header_name = strtok_r(line, ":", &saveptr_sub_token);
		char *header_value = strtok_r(NULL, "", &saveptr_sub_token);

		if(header_name && header_value) {
			printf("%s: %s\n", header_name, header_value);
		}
	}	
	//    strip_space(line, strlen(line));
        // printf("%s\n", line);
	
	line = strtok_r(NULL, "\n", &saveptr_line);
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

        char dummy_response[] = "HTTP/1.1 200 OK\r\nServer: C-Server\r\nDate: Wed, 03 Dec 2025 12:32:00 GMT\r\nContent-Length: 4\r\nContent-Type: text/html\r\nCache-Control: no-store\r\n\r\nRESP";

        send(client_socket, dummy_response, strlen(dummy_response), 0);

        close(client_socket);
    }

    int cleanup_status = SERVER_SOCKET_TERMINATE(server_socket);

    return (cleanup_status == SERVER_OK) ? EXIT_SUCCESS : EXIT_FAILURE;
}
