#include "http.h"

int main(void) {
    HTTP_SERVER *app = http_server_create("0.0.0.0", 3000);

    // register routes before running app loop

    (void) http_server_run(app);

    (void) http_server_destroy(app);
}
