seq 1 10000 | xargs -n1 -P1000 curl -s -o /dev/null -w "%{time_total}\n" http://localhost:3000/


# wrk -t3 -c20 -d5s http://127.0.0.1:3000