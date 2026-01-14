#ifndef HTTP_H
#define HTTP_H

#include <stdlib.h>
#include <stdint.h>
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

#define HTTP_BUFFER_SIZE 1000
#define MAX_METHOD_LEN 16
#define MAX_PATH_LEN 2048
#define MAX_HEADER_NAME_LEN 256
#define MAX_HEADER_VALUE_LEN 8192

enum SERVER_CODES {
    SERVER_OK = 0,
    SERVER_ERR_CLOSE = 1,
    SERVER_ERR_BIND  = 2,
    SERVER_ERR_LISTEN = 3,
    SERVER_ERR_ACCEPT = 4,
};

typedef enum HttpMethod {
    HTTP_GET,
    HTTP_POST,
    HTTP_PUT,
    HTTP_DELETE,
} HttpMethod;

typedef enum HttpProtocol {
    HTTP_1_0, // 1.0
    HTTP_1_1, // 1.1
    HTTP_2_0, // 2.0
} HttpProtocol;

typedef struct HttpRequest {
    HttpMethod method;
    mString *route;
    HttpProtocol proto;
    ArrayList *headers;
    mString *body;
} Request;

typedef struct HTTP_SERVER {
    volatile sig_atomic_t SHUTDOWN_REQ;
    const char *HOST_IP;
    uint16_t PORT;
} HTTP_SERVER;

HTTP_SERVER *http_server_create(const char *HOST_IP, const uint16_t PORT);
void http_server_destroy(HTTP_SERVER *app);
int http_server_run(HTTP_SERVER *app);

#endif