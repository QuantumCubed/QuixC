# QuixC

A minimal HTTP/1.1 server framework written from scratch in C — raw BSD sockets, no external dependencies. It provides an Express-style routing API, zero-copy static file serving, and graceful shutdown via POSIX signals.

## Features

- **From-scratch HTTP/1.1 server** on raw BSD sockets (`socket`/`bind`/`listen`/`accept`), with `SO_REUSEADDR` for fast rebinds.
- **Express-style router** — register method + route + a `(request, response)` callback; supports a catch-all route for static serving.
- **Zero-copy static file serving** via Linux `sendfile()`, with partial-send handling.
- **Polymorphic response body** — a tagged union over string, file, in-memory buffer, and `sendfile` sources.
- **Dynamic header map** with duplicate-header prevention.
- **MIME resolution** by file extension.
- **Graceful shutdown** through a `sigaction`-installed `SIGINT` handler that flips a `volatile sig_atomic_t` flag, letting the accept loop unwind and free resources cleanly.
- **Custom data structures** — a growable string type (`mString`) and a generic `ArrayList`, with explicit create/destroy memory lifecycles.

## Build & Run

```sh
# compile (adjust to your build setup)
gcc main.c lib/quixc.c lib/mstring.c lib/arraylist.c -Iinclude -o quixc

# run
./quixc
```

The server binds to the host/port passed to `quixc(...)` and serves until it receives `SIGINT` (Ctrl+C).

## Usage

```c
#include "quixc.h"

void hello(QuixC_Request *req, QuixC_Response *res) {
    res->status = QuixC_OK;
    quixc_header_add(&res->header_map, "Content-Type", "text/plain");
    // set res->body (string / file / sendfile) ...
}

int main(void) {
    QuixC *app = quixc("0.0.0.0", 8080);

    quixc_route_register(&app->router, QuixC_HTTP_GET, "/hello", hello);
    quixc_CA_route_register(&app->router, "static");  // catch-all -> static dir

    int status = quixc_run(app);
    quixc_cleanup(app);
    return status;
}
```

## Architecture

```
accept loop (quixc_run)
   -> recv request bytes
   -> quixc_request_create   (parse method / route / protocol / headers)
   -> quixc_router_handler   (match route, else catch-all static)
   -> route callback         (populate QuixC_Response)
   -> quixc_response_send     (serialize headers; send body via send()/sendfile())
   -> close client socket
```

## Roadmap

- Concurrency: move from the single-threaded blocking accept loop to a worker pool / `SO_REUSEPORT` multi-process model.
- Middleware support in the router.
- Keep-alive / connection reuse.

## License

MIT
