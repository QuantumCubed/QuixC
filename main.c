#include "http.h"

void do_something(HttpRequest *req, HttpResponse *res) {
    // add headers
    // http_response_build(res, "<h1><b>RESP</b></h1>", 200);

    http_header_add(&(res -> header_map), "content-type", "application/json");
    http_response_build(res, "{ \"status\": \"OK\" }", 200); // make this json not bs somehow
}

int main(void) {
    HTTP_SERVER *app = http_server_create("0.0.0.0", 3000);

    // register routes before running app loop

    register_route(app, HTTP_GET, "/something", do_something);

    int cleanup_status = http_server_run(app);

    http_server_destroy(app);

    return (cleanup_status == EXIT_SUCCESS) ? 0 : 1;
}
