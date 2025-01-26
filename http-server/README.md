# HTTP Server

- A server to explore and practice some C concepts.

## Goals

- [x] Non blocking I/O with kqueue and event loop to handle incomming connections.
- [x] Handle only Non-Persistent Connections (no keep-alive, always send the `Connection: close` header).
- [x] Partial writes.
  - Use a hashtable for partially writen sockets.
- [x] Use wrk to test the server. For now it can handle at least the same amount of load than a simple node server.
- [ ] Cached responses
  - There is only a static html file which is cached on first request.
  - TODO: investigate caching!
- [ ] Partial reads.
  - For now we are only handling GET requests
  - TODO: extend the server to handle other methods as well.

### Compile and run

> $`make`

or

> \$`make rebuild`

and

> `./out/http_server`

### Misc

To print the `DEBUG` logs just set the `DEBUG_C_SERVER` variable:

> $`export DEBUG_C_SERVER=1`
