const http = require('http');

const server = http.createServer((req, res) => {
    // Log the request method and URL
    console.log(`${req.method} ${req.url}`);

    // Set the response status code to 200 (OK)
    res.statusCode = 200;

    // Set the response headers
    res.setHeader('Content-Type', 'text/plain');

    // Send the response body
    res.end('OK');
});

// Define the port to listen on
const port = 3000;

// Start the server
server.listen(port, () => {
    console.log(`Server running at http://localhost:${port}/`);
});