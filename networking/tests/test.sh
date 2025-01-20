seq 1 1000 | xargs -n1 -P100 curl -s http://localhost:3000/
