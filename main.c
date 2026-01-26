#include "http.h"

void do_something(HttpRequest *req, HttpResponse *res) {
    printf("This is something!\n");
    response_send(res, "text/html", "<h1><b>RESP</b></h1>", 200);
}

int main(void) {
    HTTP_SERVER *app = http_server_create("0.0.0.0", 3000);

    // register routes before running app loop

    register_route(app, HTTP_GET, "/something", do_something);

    (void) http_server_run(app);

    (void) http_server_destroy(app);
}
