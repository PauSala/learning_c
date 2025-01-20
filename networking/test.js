const http = require('http');

const options = {
  method: 'GET',
  hostname: 'localhost',
  port: '3000',
  path: '/',
  headers: {
    Accept: '*/*',
    'User-Agent': 'Thunder Client (https://www.thunderclient.com)'
  }
};

for (let i = 0; i < 40; i++) {
    const req = http.request(options, function (res) {
        const chunks = [];

        res.on('data', function (chunk) {
            chunks.push(chunk);
        });

        res.on('end', function () {
            console.log(`Request ${i} completed`);
        });
    });

    req.on('error', (err) => {
        console.error(`Request ${i} failed:`, err);
    });

    req.end();
}