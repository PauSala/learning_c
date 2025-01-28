# HTTP Server

- A server to explore and practice some C concepts.
- I've used Brian “Beej Jorgensen” Hall [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/html/split/index.html) as a starting point.

## Warning

- This server is for (my) educational purposes only. Do not use this server in production or expose it to the world.
- Since it uses kqueue for event-driven I/O, it only compiles on macOS, FreeBSD, and other BSD-based systems (like OpenBSD and NetBSD).

## Goals

- [x] Non blocking I/O with kqueue and event loop to handle incomming connections.
- [x] Handle Non-Persistent Connections (no keep-alive, always send the `Connection: close` header).
- [x] Partial reads for large requests.
- [x] Partial writes for large responses.
- [x] Http parser
  - [x] Parser based on a state machine for allowing partial reads.
  - [x] Use kevent.udata to pass away the parser in the current state for each request.
  - [x] Parse method, url, version, headers and body.
  - [ ] TODO: Check for valid urls, version, etc. It does not perform any semantinc interpretation of the headers except for content-length.
- [x] Use wrk to test the server. For now it can handle at least the same amount of load than a simple node server.
- [ ] Cached responses
  - There is only a static html file which is cached on first request.
  - TODO: investigate caching!
- [ ] Interpret requests and respond accordingly

### Compile and run

> make

or

> make rebuild

and

> ./out/http_server

### Debug

To print the `DEBUG` logs just set the `DEBUG_C_SERVER` variable:

> export DEBUG_C_SERVER=1

### Test

- In the `tests` folder there is a bash script to run `wrk` against the server. Just uncommend the line you are interested in and run the script.
- The `server.js` is a node server to use as a benchmark when using `wrk`[^1].

[^1]: I was receiving errors with `wrk` for 12 threads and 400 connections, and then realized that Node wasn't able to handle this setup either on my computer. So, I tried a config that does not generate errors with Node and then tested it with my server.
