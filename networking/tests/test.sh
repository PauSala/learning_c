seq 1 10000 | xargs -n1 -P1000 curl -s -o /dev/null -w "%{time_total}\n" http://localhost:3000/
