#ifndef HTTP_H
#define HTTP_H

#include "mstring.h"
#include "arraylist.h"

typedef enum QuixC_Server_Codes {
    SERVER_OK = 0,
    SERVER_ERR_CLOSE = 1,
    SERVER_ERR_BIND  = 2,
    SERVER_ERR_LISTEN = 3,
    SERVER_ERR_ACCEPT = 4,
} QuixC_Server_Codes;

typedef enum QuixC_Status_Code {
    OK = 200,
    RESOURCE_NOT_FOUND = 404,
} QuixC_Status_Code;

typedef enum QuixC_Method {
    HTTP_GET,
    HTTP_POST,
    HTTP_PUT,
    HTTP_DELETE,
    HTTP_UNKNOWN
} QuixC_Method;

typedef enum QuixC_Protocol {
    HTTP_1_0, // 1.0
    HTTP_1_1, // 1.1
    HTTP_2_0, // 2.0
    PROTOCOL_UNKNOWN
} QuixC_Protocol;

typedef enum QuixC_Body_t {
    BODY_TYPE_STRING,
    BODY_TYPE_FILE,
    BODY_TYPE_BUFFER,
    BODY_TYPE_SENDFILE
} QuixC_Body_t;

typedef struct QuixC_Body {
    HttpBody_t type;
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
    HttpHeader *headers;
    size_t count;
    size_t capacity;
} QuixC_Header_Map;

typedef struct QuixC_Request {
    HttpMethod method;
    mString *route;
    HttpProtocol proto;
    HttpHeaderMap header_map;
    mString *body; // change eventually to HttpBody
} QuixC_Request;

typedef struct QuixC_Response {
    HttpProtocol proto;
    HttpStatusCode status;
    HttpHeaderMap header_map;
    HttpBody *body;
} QuixC_Response;

typedef struct QuixC_Router {
    HttpMethod method;
    const char *route_str;
    void (*route_callback)(HttpRequest *req, HttpResponse *res);
} QuixC_Router;

typedef struct QuixC {
    volatile sig_atomic_t SHUTDOWN_REQ;
    const char *HOST_IP;
    uint16_t PORT;
    ArrayList *routes;
} QuixC;

QuixC *quixc_create(const char *HOST_IP, const uint16_t PORT);
void quixc_cleanup(QuixC *app);
int quixc_run(QuixC *app);
void register_route(QuixC *app, HttpMethod method, const char *route_str, void (*callback)(HttpRequest *req, HttpResponse *res)); // void *callback(HttpRequest req)
bool http_header_add(HttpHeaderMap *map, const char *key, const char *value);

#endif