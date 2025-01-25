# Simple HTTP Server

- A server to explore and practice some C concepts.

## Done

- [x] Multiplexed connections through kqueue.
- [x] Handling partial writes for large response files.
- [x] A simple console logger.
- [x] Experimenting with a result type using macros.

## Roadmap

- [ ] Parse HTTP requests and send appropriate responses.
  - [x] naive HTTP parser
  - [ ] handle EOF
- [ ] Serve any file from the "public" folder.
- [ ] Implement caching for requested files/responses.
- [ ] Parse and handle JSON request bodies?
- [ ] Implement JSON response support?
