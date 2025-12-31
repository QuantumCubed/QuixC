#ifndef HTTP_H
#define HTTP_H

#include <stdlib.h>

#define BUFFER_SIZE 4096
#define MAX_METHOD_LEN 16
#define MAX_PATH_LEN 2048
#define MAX_HEADER_NAME_LEN 256
#define MAX_HEADER_VALUE_LEN 8192

// enum HTTP_METHOD {
//     GET,
//     POST,
//     PUT,
//     DELETE,
// };

typedef struct HTTP_REQUEST {
    char method[MAX_METHOD_LEN];
    char path[MAX_PATH_LEN];
    char version[16];
    char *body;
    uint16_t body_length;
} HTTP_REQUEST;

typedef struct HTTP_RESPONSE {
    
} HTTP_RESPONSE;

#endif