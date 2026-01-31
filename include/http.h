#ifndef HTTP_H
#define HTTP_H

#include "mstring.h"
#include "arraylist.h"
#include <stdint.h>
#include <signal.h>

typedef enum QuixC_Server_Codes {
    QuixC_SUCC_OK = 0,
    QuixC_ERR_CLOSE = 1,
    QuixC_ERR_BIND  = 2,
    QuixC_ERR_LISTEN = 3,
    QuixC_ERR_ACCEPT = 4,
} QuixC_Server_Codes;

typedef enum QuixC_Status_Code {
    QuixC_OK = 200,
    QuixC_RESOURCE_NOT_FOUND = 404,
} QuixC_Status_Code;

typedef enum QuixC_Method {
    QuixC_HTTP_GET,
    QuixC_HTTP_POST,
    QuixC_HTTP_PUT,
    QuixC_HTTP_DELETE,
    QuixC_HTTP_UNKNOWN
} QuixC_Method;

typedef enum QuixC_Protocol {
    QuixC_HTTP_1_0, // 1.0
    QuixC_HTTP_1_1, // 1.1
    QuixC_HTTP_2_0, // 2.0
    QuixC_PROTOCOL_UNKNOWN
} QuixC_Protocol;

typedef enum QuixC_Body_t {
    BODY_TYPE_STRING,
    BODY_TYPE_FILE,
    BODY_TYPE_BUFFER,
    BODY_TYPE_SENDFILE
} QuixC_Body_t;

typedef struct QuixC_Body {
    QuixC_Body_t type;
    size_t content_length;
    union {
        const char *string;
        struct {
            const char *filepath;
        } file;
        struct {
            void *binary;
        } buffer;
    } content;
} QuixC_Body;

typedef struct QuixC_Header {
    mString *key;
    mString *value;
} QuixC_Header;

typedef struct QuixC_Header_Map {
    QuixC_Header *headers;
    size_t count;
    size_t capacity;
} QuixC_Header_Map;

typedef struct QuixC_Request {
    QuixC_Method method;
    mString *route;
    QuixC_Protocol proto;
    QuixC_Header_Map header_map;
    mString *body; // change eventually to HttpBody
} QuixC_Request;

typedef struct QuixC_Response {
    QuixC_Protocol proto;
    QuixC_Status_Code status;
    QuixC_Header_Map header_map;
    QuixC_Body *body;
} QuixC_Response;

typedef struct QuixC_Route {
    QuixC_Method method;
    const char *route_str;
    void (*route_callback)(QuixC_Request *req, QuixC_Response *res);
} QuixC_Route;

typedef struct QuixC {
    volatile sig_atomic_t SHUTDOWN_REQ;
    const char *HOST_IP;
    uint16_t PORT;
    ArrayList *routes;
} QuixC;

QuixC *quixc_create(const char *HOST_IP, const uint16_t PORT);
void quixc_cleanup(QuixC *app);
void quixc_route_register(QuixC *app, QuixC_Method method, const char *route_str, void (*callback)(QuixC_Request *req, QuixC_Response *res)); // void *callback(HttpRequest req)
int quixc_run(QuixC *app);
bool quixc_header_add(QuixC_Header_Map *map, const char *key, const char *value);

#endif