#include "http.h"
#include <stdio.h>
#include <sys/stat.h>

void do_something(HttpRequest *req, HttpResponse *res) {
    // add headers
    // http_response_build(res, "<h1><b>RESP</b></h1>", 200);

    http_header_add(&(res -> header_map), "content-type", "application/json");
    http_header_add(&(res -> header_map), "cache-control", "no-store");

    res -> body -> content.string = "{ \"status\": \"OK\" }";  // make this json not bs somehow
    res -> body -> content_length = strlen(res -> body -> content.string);
    res -> body -> type = BODY_TYPE_STRING;
    res -> status = 200;
}

void homepage(HttpRequest *req, HttpResponse *res) {
    res -> body -> content.file.filepath = "./static/index.html";
    res -> body -> type = BODY_TYPE_SENDFILE;

    struct stat st;
    
    if(stat("./static/index.html", &st) == 0) {
        res -> body -> content_length = st.st_size;
    }

    res -> status = 200;
}

int main(void) {
    HTTP_SERVER *app = http_server_create("0.0.0.0", 3000);

    // register routes before running app loop

    register_route(app, HTTP_GET, "/something", do_something);
    register_route(app, HTTP_GET, "/", homepage);

    int cleanup_status = http_server_run(app);

    http_server_destroy(app);

    return (cleanup_status == EXIT_SUCCESS) ? 0 : 1;
}
