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
#include <time.h>

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

    printf("\nThe Server Is Listening On:\nIP: %s\nPORT: %u\n\n", app -> HOST_IP, app -> PORT);

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

HttpMethod parse_method(const char *method_str) {
    if (strcmp(method_str, "GET") == 0) return HTTP_GET;
    if (strcmp(method_str, "POST") == 0) return HTTP_POST;
    if (strcmp(method_str, "PUT") == 0) return HTTP_PUT;
    if (strcmp(method_str, "DELETE") == 0) return HTTP_DELETE;
    return HTTP_UNKNOWN;    
}

HttpProtocol parse_protocol(const char *protocol_str) {
    if (strcmp(protocol_str, "HTTP/1.0") == 0) return HTTP_1_0;
    if (strcmp(protocol_str, "HTTP/1.1") == 0) return HTTP_1_1;
    if (strcmp(protocol_str, "HTTP/2.0") == 0) return HTTP_2_0;
    return PROTOCOL_UNKNOWN;
}

void register_route(HTTP_SERVER *app, HttpMethod method, const char *route_str, void (*callback)(HttpRequest *req, HttpResponse *res)) { // (req, res) : const void *callback(HttpRequest req)
    
    Route route;

    route.method = method;
    route.route_str = route_str;
    route.route_callback = callback;
    
    arraylist_append(app -> routes, &route); // should be fine due to memcpy arraylist impl *I think?*
}

const char* protocol_to_string(HttpProtocol protocol) {
    switch (protocol) {
        case HTTP_1_0: return "HTTP/1.0";
        case HTTP_1_1: return "HTTP/1.1";
        case HTTP_2_0: return "HTTP/2.0";
        default: return "HTTP/1.1";  // fallback
    }
}

const char* status_code_to_string(int status_code) {
    switch (status_code) {
        case 200: return "200 OK";
        case 201: return "201 Created";
        case 400: return "400 Bad Request";
        case 404: return "404 Not Found";
        case 500: return "500 Internal Server Error";
        default: return "500 Internal Server Error";
    }
}

void response_send(HttpResponse *res, char *body_t, char *body, int status) {

    res -> status = status;

    char res_buffer[1000];
    char date_buffer[64];

    time_t now = time(NULL);
    struct tm *gmt = gmtime(&now);

    strftime(date_buffer, sizeof(date_buffer), "%a, %d %b %Y %H:%M:%S GMT", gmt);

    snprintf(res_buffer, 1000,
        "%s %s\r\n"
        "Server: C-Server\r\n"
        "Date: %s\r\n"
        "Content-Length: %zu\r\n"
        "Content-Type: %s\r\n"
        "Cache-Control: no-store\r\n"
        "\r\n"
        "%s",
        protocol_to_string(res -> proto),
        status_code_to_string(res -> status),
        date_buffer,
        strlen(body),
        body_t,
        body
    );

    printf("\n********* RESPONSE MESSAGE *********\n");

    printf("\n%s\n", res_buffer);

    printf("********* **************** *********\n");
}

void parse_route(HTTP_SERVER *app, HttpRequest *req, HttpResponse *res) {

    ArrayList *routes = app -> routes;
    int route_index = -1;

    // eventually switch to a memory mapping with route as key instead of looping through
    // might be multiple instances of the same ROUTE with different METHODS
    
    for(size_t i = 0; i < routes -> size; ++i) {
        const char *route_str = ((Route *) arraylist_get(routes, i)) -> route_str;
        if(strcmp((req -> route) -> chars, route_str) == 0) { // strcmp returns 0 if equal for some reason -_-
            route_index = i;
        }
    }

    if(route_index < 0) return;

    Route *rte_ptr = (Route *) arraylist_get(routes, route_index);

    if(req -> method != rte_ptr -> method) return; // if HTTP Method between registered & req do not match

    rte_ptr -> route_callback(req, res);

}

void parse_request(HTTP_SERVER *app, mString *str) {

    HttpRequest req;
    HttpResponse res;

    res.sent = false; // add "sent checking"; if callback fn defined by user forgets to send a response, send a default response

    printf("Request Length: %zu\n", str -> length);
    // puts(str -> chars);

    ArrayList *tokens = m_string_tokenize(str, "\r\n");

    printf("Number of Tokens: %zu\n\n", tokens -> size);

    mString *sl_tok = *(mString **) arraylist_get(tokens, 0);

    ArrayList *sl_tok_subtokens = m_string_tokenize(sl_tok, " ");

    if(sl_tok_subtokens -> size == 3) {
        printf("Method: %s\nRoute: %s\nProtocol: %s\n\n", (*(mString **) arraylist_get(sl_tok_subtokens, 0)) -> chars, (*(mString **) arraylist_get(sl_tok_subtokens, 1)) -> chars, (*(mString **) arraylist_get(sl_tok_subtokens, 2)) -> chars);
        req.method = parse_method((*(mString **) arraylist_get(sl_tok_subtokens, 0)) -> chars);
        req.route = (*(mString **) arraylist_get(sl_tok_subtokens, 1));
        req.proto = parse_protocol((*(mString **) arraylist_get(sl_tok_subtokens, 2)) -> chars);
    }

    for(size_t i = 1; i < (tokens -> size) - 2; ++i) { // skip start line, empty line, and body (headers only)
        mString *tok_ptr = *(mString **) arraylist_get(tokens, i);
        ArrayList *subtokens = m_string_tokenize(tok_ptr, ": ");

        printf("%s: ", (*(mString **) arraylist_get(subtokens, 0)) -> chars);

        if(subtokens -> size > 1) {
            printf("%s\n", (*(mString **) arraylist_get(subtokens, 1)) -> chars);
        } else {
            printf("\n"); // no delimeter for subtoken e.g. empty line between header & body, req body itself
        }
        arraylist_destroy(subtokens); // eventually add to arraylist and let arraylist take ownership
    }

    printf("\nEmpty Line: %s\n", (*(mString **) arraylist_get(tokens, (tokens -> size) - 2)) -> chars);
    printf("Body: %s\n\n", (*(mString **) arraylist_get(tokens, (tokens -> size) - 1)) -> chars);

    req.body = (*(mString **) arraylist_get(tokens, (tokens -> size) - 1));

    // for(size_t i = 0; i < tokens -> size; ++i) {
    //     mString *tok_ptr = *(mString **) arraylist_get(tokens, i);
    //     if(i + 1 == tokens -> size) {
    //         printf("%s\n", tok_ptr -> chars);
    //         break;
    //     }
    //     printf("%s\n***TOKEN***\n", tok_ptr -> chars);
    // }

    printf("********* REQUEST STRUCT *********\n");
    printf("%d\n", req.method);
    puts(req.route -> chars);
    printf("%d\n", req.proto);
    // puts(req.headers);
    puts(req.body -> chars);
    printf("********* ************** *********\n\n");

    parse_route(app, &req, &res);

    arraylist_destroy(sl_tok_subtokens);
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

    app -> routes = arraylist_create(10, sizeof(Route), NULL); // no cleanup callbacks required (all members are stack)

    return app;
}

void http_server_destroy(HTTP_SERVER *app) {
    if(!app) return;

    arraylist_destroy(app -> routes); // cleanup routes arraylist
    free(app);
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
        parse_request(app, &buffer);

        char dummy_response[] = "HTTP/1.1 200 OK\r\nServer: C-Server\r\nDate: Wed, 03 Dec 2025 12:32:00 GMT\r\nContent-Length: 4\r\nContent-Type: text/html\r\nCache-Control: no-store\r\n\r\nRESP";

        send(client_socket, dummy_response, strlen(dummy_response), 0);

        close(client_socket);
    }

    int cleanup_status = SERVER_SOCKET_TERMINATE(server_socket);

    return (cleanup_status == SERVER_OK) ? EXIT_SUCCESS : EXIT_FAILURE;
}