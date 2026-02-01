#include "quixc.h"
#include "mstring.h"
#include "arraylist.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <fcntl.h>       // For open() and O_RDONLY
#include <sys/stat.h>    // For fstat() and struct stat
#include <unistd.h>      // For close() and lseek()
#include <sys/sendfile.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <strings.h>
#include <signal.h>
#include <ctype.h>
#include <stdint.h>
#include <time.h>

#define SOCKET_BUFFER_MAX 8192

#define M_STRING_ARR_GET(mString_ArrayList, index) *(mString **) arraylist_get(mString_ArrayList, index)

volatile sig_atomic_t GLOBAL_SHUTDOWN_REQ = 0;

// *************************** SOCKET FUNCTIONS *************************** //

static void QUIXC_SIGNAL_HANDLER(int sig) {
    printf("\nSIG: %d\n", sig);
    GLOBAL_SHUTDOWN_REQ = 1;
}

static int QUIXC_SOCKET_INIT(QuixC *app) {
    struct sockaddr_in address;

    int server_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (server_socket == -1) {
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

static int QUIXC_SOCKET_TERMINATE(int server_socket_fd) {
    printf("SHUTTING SERVER DOWN...\n");

    if(close(server_socket_fd) == -1) {
        int error = errno;
        fprintf(stderr, "GRACEFUL SHUTDOWN UNSUCCESSFUL!\n");
        perror("socket_shutdown");
        return QuixC_ERR_CLOSE;
    }

    printf("GRACEFUL SHUTDOWN SUCCESSFUL!\n");
    return 0;
}

// *************************** **************** *************************** //

// *************************** PARSE_FUNCTIONS *************************** //

QuixC_Method quixc_method_parse(const char *method_str) {
    if (strcmp(method_str, "GET") == 0) return QuixC_HTTP_GET;
    if (strcmp(method_str, "POST") == 0) return QuixC_HTTP_POST;
    if (strcmp(method_str, "PUT") == 0) return QuixC_HTTP_PUT;
    if (strcmp(method_str, "DELETE") == 0) return QuixC_HTTP_DELETE;
    return QuixC_HTTP_UNKNOWN;    
}

QuixC_Protocol quixc_protocol_parse(const char *protocol_str) {
    if (strcmp(protocol_str, "HTTP/1.0") == 0) return QuixC_HTTP_1_0;
    if (strcmp(protocol_str, "HTTP/1.1") == 0) return QuixC_HTTP_1_1;
    if (strcmp(protocol_str, "HTTP/2.0") == 0) return QuixC_HTTP_2_0;
    return QuixC_PROTOCOL_UNKNOWN;
}

const char* quixc_proto_to_str(QuixC_Protocol protocol) {
    switch (protocol) {
        case QuixC_HTTP_1_0: return "HTTP/1.0";
        case QuixC_HTTP_1_1: return "HTTP/1.1";
        case QuixC_HTTP_2_0: return "HTTP/2.0";
        default: return "HTTP/1.1";  // fallback
    }
}

const char* quixc_sc_to_str(int status_code) {
    switch (status_code) {
        case 200: return "200 OK";
        case 201: return "201 Created";
        case 400: return "400 Bad Request";
        case 404: return "404 Not Found";
        case 500: return "500 Internal Server Error";
        default: return "500 Internal Server Error";
    }
}

// *************************** *************** *************************** //

// *************************** HTTP HEADER FUNCTIONS *************************** //


bool quixc_header_init(QuixC_Header_Map *map) {
    map -> capacity = 16;
    map -> count = 0;
    map -> headers = (QuixC_Header *) malloc(map -> capacity * sizeof(QuixC_Header));

        if(!(map -> headers)) {
         // ERROR
        fprintf(stderr, "HttpHeader Alloc Error!\n");
        return false;
    }
    return true;
}

void quixc_header_map_cleanup(QuixC_Header_Map *map) {

    if(!map) {
        fprintf(stderr, "NULL POINTER!\n");
        return;
    }

    for(size_t i = 0; i < map -> count; ++i) {
        m_string_destroy(map -> headers[i].key);
        m_string_destroy(map -> headers[i].value);
    }
    free(map -> headers);
}

// RETURNS INDEX IF MATCH, -1 IF NOT FOUND
ssize_t quixc_header_map_contains(QuixC_Header_Map *map, const char *header) {
    for(size_t i = 0; i < map -> count; ++i) {
        if(strcasecmp(map -> headers[i].key -> chars, header) == 0) {
            return i;
        }
    }
    return -1;
}

bool quixc_header_add(QuixC_Header_Map *map, const char *key, const char *value) {
    if((map -> count) + 1 >= map -> capacity) {
        size_t x2_capacity = (map -> capacity) * 2;
        map -> headers = realloc(map -> headers, sizeof(QuixC_Header) * x2_capacity);
        if(!(map -> headers)) {
            fprintf(stderr, "HEADER REALLOC FAIL!\n");
            return false;
        }
        map -> capacity = x2_capacity;
    }

    // (prevents duplicate headers)

    ssize_t key_index = quixc_header_map_contains(map, key);

    if(key_index != -1) {
        m_string_destroy(map -> headers[key_index].value);
        map -> headers[key_index].value = map -> headers[map -> count].value = m_string_from_cstr(value);
        return true;
    }

    map -> headers[map -> count].key = m_string_from_cstr(key);
    map -> headers[map -> count].value = m_string_from_cstr(value);
    map -> count++;

    return true;
}

// *************************** ********************* *************************** //

// *************************** HTTP REQUEST FUNCTIONS *************************** //

// HTTP REQUEST MUST TAKE OWNERSHIP FOR ALL HEAP-ALLOCATED MEMBERS/SUB-MEMBERS
QuixC_Request *quixc_request_create(mString *req_buffer) {
    QuixC_Request *req = (QuixC_Request *) malloc(sizeof(QuixC_Request));

    req -> route = NULL;
    req -> body = NULL;

    if(!req) {
        // ERROR
        fprintf(stderr, "HttpRequest Alloc Error!\n");
        return NULL;
    }

    if(!(quixc_header_init(&(req -> header_map)))) { free(req); return NULL; }

    printf("Request Length: %zu\n", req_buffer -> length);

    ArrayList *sections = m_string_tokenize(req_buffer, "\r\n\r\n");

    printf("Number of Sections: %zu\n", sections -> size);

    ArrayList *lines = m_string_tokenize(M_STRING_ARR_GET(sections, 0), "\r\n");

    printf("Number of Lines: %zu\n\n", lines -> size);

    // PARSE START LINE

    ArrayList *sl_tok_subtokens = m_string_tokenize(M_STRING_ARR_GET(lines, 0), " ");

    if(!sl_tok_subtokens || sl_tok_subtokens -> size != 3) {
        fprintf(stderr, "SL_TOK_ERROR!\n");
        arraylist_destroy(lines);
        arraylist_destroy(sections);
        return NULL;
    }

    printf("Method: %s\nQuixC_Route: %s\nProtocol: %s\n\n", (M_STRING_ARR_GET(sl_tok_subtokens, 0)) -> chars, (M_STRING_ARR_GET(sl_tok_subtokens, 1)) -> chars, (M_STRING_ARR_GET(sl_tok_subtokens, 2)) -> chars);
    
    req -> method = quixc_method_parse((M_STRING_ARR_GET(sl_tok_subtokens, 0)) -> chars);
    req -> route = m_string_dup((M_STRING_ARR_GET(sl_tok_subtokens, 1)));
    req -> proto = quixc_protocol_parse((M_STRING_ARR_GET(sl_tok_subtokens, 2)) -> chars);
    
    arraylist_destroy(sl_tok_subtokens);

    // PARSE HEADERS
    
    for(size_t i = 1; i < lines -> size; ++i) {
        ArrayList *header_line = m_string_tokenize(M_STRING_ARR_GET(lines, i), ":");

        if((header_line -> size) > 1) {
            mString *key = M_STRING_ARR_GET(header_line, 0);
            mString *value = M_STRING_ARR_GET(header_line, 1);

            m_string_trim_leading_whitespace(key);
            m_string_trim_leading_whitespace(value);

            if(!(quixc_header_add(&(req -> header_map), key -> chars, value -> chars))) {
                arraylist_destroy(header_line);
                return NULL;
            }
            // printf("%s: %s\n", key -> chars, value -> chars);
        }
        arraylist_destroy(header_line);
    }
    
    // CONTENT-LENGTH HEADER (SPECIFICALLY)

    ssize_t found = quixc_header_map_contains(&(req -> header_map), "content-length");
    ssize_t body_length;


    if(found != -1) {
        body_length = (size_t) strtoul(req -> header_map.headers[found].value -> chars, NULL, 10);
    } else {
        body_length = 0;
    }

    // PARSE BODY
    // if method has body allocate space...
    if(req -> method != QuixC_HTTP_GET) {
        req -> body = m_string(body_length);
    }

    arraylist_destroy(lines);
    arraylist_destroy(sections);

    return req;
}

void quixc_request_destroy(QuixC_Request *req) {
    if(!req) {
        // problem
        fprintf(stderr, "Request Cleanup Error!\n");
        return;
    }

    m_string_destroy(req -> route);

    quixc_header_map_cleanup(&(req -> header_map));
    
    if(req -> body) {
        m_string_destroy(req -> body);
    }

    free(req);
}

void quixc_request_execute(const QuixC_Route *route, QuixC_Request *req, QuixC_Response *res) {
    
    // add protocol verification etc (unless that is in http_request_router)
    res -> proto = req -> proto;
    route -> route_callback(req, res);
}

// *************************** ********************** *************************** //

// *************************** HTTP RESPONSE FUNCTIONS *************************** //

QuixC_Response *quixc_response_create() {
    QuixC_Response *res = (QuixC_Response *) malloc(sizeof(QuixC_Response));

    if(!res) {
        fprintf(stderr, "HTTP RES ALLOC FAIL!\n");
        return NULL;
    }

    res -> body = (QuixC_Body *) malloc(sizeof(QuixC_Body));

    if(!(res -> body)) {
        fprintf(stderr, "HTTP RES-BODY ALLOC FAIL!\n");
        free(res);
        return NULL;
    }

    if(!(quixc_header_init(&(res -> header_map)))) {
        free(res);
        return NULL;
    }

    // DEFAULT RESPONSE HEADERS

    quixc_header_add(&(res -> header_map), "content-type", "text/html");

    return res;
}

void quixc_response_destroy(QuixC_Response *res) {
    if(!res) {
        fprintf(stderr, "NULL POINTER!\n");
        return;
    }

    // maybe if check

    free(res -> body);

    // res -> body memebers that are heap allocated need to be freed

    quixc_header_map_cleanup(&(res -> header_map));

    // m_string_destroy(res -> body);
}

void quixc_response_send(int client_socket, QuixC_Response *res) {

    char header_buffer[4096]; // switch to defined macro size
    char date_buffer[64];

    time_t now = time(NULL);
    struct tm *gmt = gmtime(&now);

    strftime(date_buffer, sizeof(date_buffer), "%a, %d %b %Y %H:%M:%S GMT", gmt);

    // ssize_t default_content_type = http_header_map_contains(&(res -> header_map), "content-type");
    // (default_content_type == -1) ? "text/html" : res -> header_map.headers[default_content_type].value -> chars

    int offset = 0;

    // start line

    offset += snprintf(header_buffer + offset, sizeof(header_buffer) - offset,
        "%s %s\r\n",
        quixc_proto_to_str(res -> proto),
        quixc_sc_to_str(res -> status)
    );

    // server-defined headers

    offset += snprintf(header_buffer + offset, sizeof(header_buffer) - offset,
        "Server: QuixC\r\n"
        "Date: %s\r\n"
        "Content-Length: %zu\r\n",
        date_buffer,
        res -> body ? res -> body -> content_length : 0
    );

    // dynamically add user-defined headers

    for(size_t i = 0; i < res -> header_map.count; ++i) {
        offset += snprintf(header_buffer + offset, sizeof(header_buffer) - offset,
            "%s: %s\r\n",
            res -> header_map.headers[i].key -> chars,
            res -> header_map.headers[i].value -> chars
        );
    }

    // end headers

    offset += snprintf(header_buffer + offset, sizeof(header_buffer) - offset, "\r\n");

    // switch send to writev (eventually)

    (void) send(client_socket, header_buffer, offset, 0);

    switch(res -> body -> type) {
        
        case BODY_TYPE_STRING:
            (void) send(client_socket, res -> body -> content.string, res -> body-> content_length, 0);
        break;

        case BODY_TYPE_SENDFILE:
            int file_fd = open(res -> body -> content.file.filepath, O_RDONLY);

            if(file_fd == -1) {
                perror("File Open Fail!");
                return;
            }

            off_t offset = 0;
            ssize_t sent;
            while(offset < res -> body -> content_length) {
                sent = sendfile(client_socket, file_fd, &offset, res -> body -> content_length - offset);
                if(sent <= 0) break;
            }
            close(file_fd);
        break;

        case BODY_TYPE_BUFFER:
        // IMPLEMENT LATER

        break;
        
        default:
        // ERROR
        return;
    }
}

// *************************** *********************** *************************** //

// *************************** QUIXC INSTANCE FUNCTIONS *************************** //

QuixC *quixc_create(const char *HOST_IP, const uint16_t PORT) {

    QuixC *app = (QuixC *) malloc(sizeof(QuixC));

    if(!app) {
        fprintf(stderr, "Failed to start HTTP instance!\n");
        return NULL;
    }

    struct sigaction SIGACTION_ARGS;
    SIGACTION_ARGS.sa_handler = QUIXC_SIGNAL_HANDLER;
    sigemptyset(&SIGACTION_ARGS.sa_mask);
    SIGACTION_ARGS.sa_flags = 0;   // IMPORTANT: no SA_RESTART

    sigaction(SIGINT, &SIGACTION_ARGS, NULL);

    app -> SHUTDOWN_REQ = 0;
    app -> HOST_IP = HOST_IP;
    app -> PORT = PORT;

    app -> routes = arraylist_create(10, sizeof(QuixC_Route), NULL); // no cleanup callbacks required (all members are stack)

    return app;
}

void quixc_cleanup(QuixC *app) {
    if(!app) return;

    arraylist_destroy(app -> routes); // cleanup routes arraylist
    free(app);
}

void quixc_route_register(QuixC *app, QuixC_Method method, const char *route_str, void (*callback)(QuixC_Request *req, QuixC_Response *res)) { // (req, res) : const void *callback(HttpRequest req)
    
    QuixC_Route route;

    route.method = method;
    route.route_str = route_str;
    route.route_callback = callback;
    
    arraylist_append(app -> routes, &route); // should be fine due to memcpy arraylist impl *I think?*
}

void quixc_directory_register() {
    return; // add stuff
}

// REFACTOR TO QuixC ROUTER
QuixC_Route *quixc_request_router(ArrayList *app_routes, QuixC_Request *req) {
    int route_index = -1;

    // 1. Find QuixC_Route ... 2. Ensure QuixC_Route Requirements Met ...
    
    for(size_t i = 0; i < app_routes -> size; ++i) {
        const char *route_str = ((QuixC_Route *) arraylist_get(app_routes, i)) -> route_str;
        if(strcmp((req -> route) -> chars, route_str) == 0) { // strcmp returns 0 if equal for some reason -_-
            route_index = i;
        }
    }

    if(route_index < 0) return NULL;

    QuixC_Route *rte_ptr = (QuixC_Route *) arraylist_get(app_routes, route_index);

    if(req -> method != rte_ptr -> method) return NULL; // if HTTP Method between registered & req do not match

    // RETURN -1 if auth headers or other required params for request are not in request

    return rte_ptr;
}

int quixc_run(QuixC *app) {

    int server_socket = QUIXC_SOCKET_INIT(app);

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

        mString *request_buffer = m_string(SOCKET_BUFFER_MAX);

        ssize_t bytes = recv(client_socket, request_buffer -> chars, SOCKET_BUFFER_MAX - 1, 0);

        if(bytes > 0) {
            request_buffer -> chars[bytes] = '\0';
            request_buffer -> length = bytes;
        } else {
            continue;
        }

        // do some bounds checking e.g. realloc if needed

        QuixC_Request *req = quixc_request_create(request_buffer);
        QuixC_Response *res = quixc_response_create();

        const QuixC_Route *route = quixc_request_router(app -> routes, req);

        if(!route) {
            // route to ERROR RESPONSE
            // ERROR_404();
            // ADD CLEANUP
            continue;
        }
        
        quixc_request_execute(route, req, res);
        quixc_response_send(client_socket, res);

        quixc_request_destroy(req);
        quixc_response_destroy(res);
        m_string_destroy(request_buffer);

        close(client_socket);
    }

    int cleanup_status = QUIXC_SOCKET_TERMINATE(server_socket);

    return (cleanup_status == QuixC_SUCC_OK) ? EXIT_SUCCESS : EXIT_FAILURE;
}

// *************************** ************************ *************************** //
