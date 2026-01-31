#include "http.h"
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void do_something(QuixC_Request *req, QuixC_Response *res) {
    // add headers
    // http_response_build(res, "<h1><b>RESP</b></h1>", 200);

    quixc_header_add(&(res -> header_map), "content-type", "application/json");
    quixc_header_add(&(res -> header_map), "cache-control", "no-store");

    res -> body -> content.string = "{ \"status\": \"OK\" }";  // make this json not bs somehow
    res -> body -> content_length = strlen(res -> body -> content.string);
    res -> body -> type = BODY_TYPE_STRING;
    res -> status = 200;
}

void homepage(QuixC_Request *req, QuixC_Response *res) {
    res -> body -> content.file.filepath = "./static/index.html";
    res -> body -> type = BODY_TYPE_SENDFILE;

    struct stat st;

    if(stat("./static/index.html", &st) == 0) {
        res -> body -> content_length = st.st_size;
    }

    res -> status = 200;
}

int main(void) {
    QuixC *app = quixc_create("0.0.0.0", 3000);

    // register routes before running app loop

    quixc_route_register(app, QuixC_HTTP_GET, "/", homepage);
    quixc_route_register(app, QuixC_HTTP_GET, "/something", do_something);

    int cleanup_status = quixc_run(app);

    quixc_cleanup(app);

    return (cleanup_status == EXIT_SUCCESS) ? 0 : 1;
}
