#include "http.h"
#include <stdio.h>
#include <fcntl.h>       // For open() and O_RDONLY
#include <sys/stat.h>    // For fstat() and struct stat
#include <unistd.h>      // For close() and lseek()

void do_something(HttpRequest *req, HttpResponse *res) {
    // add headers
    // http_response_build(res, "<h1><b>RESP</b></h1>", 200);

    http_header_add(&(res -> header_map), "content-type", "application/json");
    http_header_add(&(res -> header_map), "cache-control", "no-store");

    http_response_build(res, "{ \"status\": \"OK\" }", 200); // make this json not bs somehow
}

// void homepage(HttpRequest *req, HttpResponse *res) {
//     int fd = open("./static/hello.html", O_RDONLY);

//     if(fd == -1) {
//         perror("File Open Fail!");
//         return;
//     }

//     struct stat st;

//     if(fstat(fd, &st) == -1) {
//         perror("fstat");
//         close(fd);
//         return;
//     }

//     size_t file_size = st.st_size;

//     http_response_build(res, NULL, 200);
// }

int main(void) {
    HTTP_SERVER *app = http_server_create("0.0.0.0", 3000);

    // register routes before running app loop

    register_route(app, HTTP_GET, "/something", do_something);
    // register_route(app, HTTP_GET, "/", homepage);

    int cleanup_status = http_server_run(app);

    http_server_destroy(app);

    return (cleanup_status == EXIT_SUCCESS) ? 0 : 1;
}
