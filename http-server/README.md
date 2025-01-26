# HTTP Server

- A server to explore and practice some C concepts.

## Warning

- This server is for (my) educational purposes only. Do not use this server in production or expose it to the world.
- Since it uses kqueue for event-driven I/O, it only compiles on macOS, FreeBSD, and other BSD-based systems (like OpenBSD and NetBSD).

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
- [ ] Http parser
  - For now, it is just a naive parser with minimal error handling.

### Compile and run

> make

or

> make rebuild

and

> ./out/http_server

### Debug

To print the `DEBUG` logs just set the `DEBUG_C_SERVER` variable:

> $`export DEBUG_C_SERVER=1`

### Test

- In the `tests` folder there is a bash script to run `wrk` against the server. Just uncommend or copy and run the line you are interested in.
- The `server.js` is a node server to use as a benchmark when using `wrk`[^1].

[^1]: I was receiving errors with `wrk` for 12 threads and 400 connections, and then realized that Node wasn't able to handle this setup either on my computer. So, I tried a config that does not generate errors with Node and then tested it with my server.
