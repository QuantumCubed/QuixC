#include "http.h"
#include "mstring.h"
#include "arraylist.h"
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
#include <ctype.h>
#include <stdint.h>

// volatile sig_atomic_t SHUTDOWN_REQ = 0;
// const char *IP = "0.0.0.0";
// const uint16_t PORT = 3000;

volatile sig_atomic_t GLOBAL_SHUTDOWN_REQ = 0;

static void SIGNAL_HANDLER(int sig) {
    printf("\nSIG: %d\n", sig);
    GLOBAL_SHUTDOWN_REQ = 1;
}

static int SERVER_SOCKET_INIT(HTTP_SERVER *app) {
    struct sockaddr_in address;
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (server_socket == -1)
    {
        fprintf(stderr, "Socket Init Error!\n");
        exit(EXIT_FAILURE); // CHANGE THIS
    }

    int opt = 1;

    if(setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) { // eventually switch to SO_REUSEPORT for multi-processing
        perror("Set Socket Options!");
    }

    address.sin_family = AF_INET;
    address.sin_port = htons(app -> PORT);
    address.sin_addr.s_addr = inet_addr(app -> HOST_IP);

    int bind_status = bind(server_socket, (struct sockaddr *)&address, sizeof(address));

    if(bind_status == -1) {
        fprintf(stderr, "Bind Error!\n");
        close(server_socket);
        exit(1);
    }

    int listen_status = listen(server_socket, 3);

    if(listen_status == -1) {
        fprintf(stderr, "Listen Error!\n");
        close(server_socket);
        exit(1);
    }

    printf("The Server Is Listening On:\nIP: %s\nPORT: %u\n\n", app -> HOST_IP, app -> PORT);

    return server_socket;
}

static int SERVER_SOCKET_TERMINATE(int server_socket_fd) {
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

void req_handler(mString *str) {

    printf("Request Length: %zu\n", str -> length);
    // puts(str -> chars);

    ArrayList *tokens = m_string_tokenize(str, "\r\n");

    printf("Number of Tokens: %zu\n\n", tokens -> size);

    mString *curr_tok = *(mString **) arraylist_get(tokens, 0);

    ArrayList *curr_tok_subtokens = m_string_tokenize(curr_tok, " ");

    printf("Subtokens: %zu\n", curr_tok_subtokens -> size);
    printf("Method: %s\nRoute: %s\nProtocol: %s\n\n", (*(mString **) arraylist_get(curr_tok_subtokens, 0)) -> chars, (*(mString **) arraylist_get(curr_tok_subtokens, 1)) -> chars, (*(mString **) arraylist_get(curr_tok_subtokens, 2)) -> chars);

    arraylist_destroy(curr_tok_subtokens);

    for(size_t i = 1; i < (tokens -> size) - 2; ++i) { // skip start line, empty line, and body (headers only)
        mString *tok_ptr = *(mString **) arraylist_get(tokens, i);
        ArrayList *subtokens = m_string_tokenize(tok_ptr, ": ");

        printf("%s: ", (*(mString **) arraylist_get(subtokens, 0)) -> chars);

        if(subtokens -> size > 1) {
            printf("%s\n", (*(mString **) arraylist_get(subtokens, 1)) -> chars);
        } else {
            printf("\n"); // no delimeter for subtoken e.g. empty line between header & body, req body itself
        }
        arraylist_destroy(subtokens);
    }

    // for(size_t i = 0; i < tokens -> size; ++i) {
    //     mString *tok_ptr = *(mString **) arraylist_get(tokens, i);
    //     if(i + 1 == tokens -> size) {
    //         printf("%s\n", tok_ptr -> chars);
    //         break;
    //     }
    //     printf("%s\n***TOKEN***\n", tok_ptr -> chars);
    // }
    arraylist_destroy(tokens);
}

HTTP_SERVER *http_server_create(const char *HOST_IP, const uint16_t PORT) {

    HTTP_SERVER *app = (HTTP_SERVER *) malloc(sizeof(HTTP_SERVER));

    if(!app) {
        fprintf(stderr, "Failed to start HTTP instance!\n");
        return NULL;
    }

    struct sigaction SIGACTION_ARGS;
    SIGACTION_ARGS.sa_handler = SIGNAL_HANDLER;
    sigemptyset(&SIGACTION_ARGS.sa_mask);
    SIGACTION_ARGS.sa_flags = 0;   // IMPORTANT: no SA_RESTART

    sigaction(SIGINT, &SIGACTION_ARGS, NULL);

    app -> SHUTDOWN_REQ = 0;
    app -> HOST_IP = HOST_IP;
    app -> PORT = PORT;

    return app;
}

void http_server_destroy(HTTP_SERVER *self) {
    if(!self) return;

    free(self);
}

int http_server_run(HTTP_SERVER *app) {

    int server_socket = SERVER_SOCKET_INIT(app);

    while(!GLOBAL_SHUTDOWN_REQ)
    {
        int client_socket = accept(server_socket, NULL, NULL);

        if(client_socket < 0) {
            if(errno == EINTR) {
                break; // sigint - end loop
            }
            perror("Some Error With Client Socket!");
            continue;
        }

        // char buffer[BUFFER_SIZE];

        Str buffer = m_str(HTTP_BUFFER_SIZE);

        recv(client_socket, buffer.chars, HTTP_BUFFER_SIZE - 1, 0);

        buffer.length = strlen(buffer.chars);
        req_handler(&buffer);

        char dummy_response[] = "HTTP/1.1 200 OK\r\nServer: C-Server\r\nDate: Wed, 03 Dec 2025 12:32:00 GMT\r\nContent-Length: 4\r\nContent-Type: text/html\r\nCache-Control: no-store\r\n\r\nRESP";

        send(client_socket, dummy_response, strlen(dummy_response), 0);

        close(client_socket);
    }

    int cleanup_status = SERVER_SOCKET_TERMINATE(server_socket);

    return (cleanup_status == SERVER_OK) ? EXIT_SUCCESS : EXIT_FAILURE;
}