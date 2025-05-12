#!/bin/bash

# Cleanup
rm -f *.time.log
pkill -f "./server" 2>/dev/null
make && ./server &
SERVER_PID=$!
sleep 1  # Let server start

# Timestamp function
ts() { date +%s.%N; }

# Launch 9 clients with 3 timestamps each (27 total files)
run_client() {
    local id=$1
    local type=$2
    local client="./${type}-client"
    
    ts > ${type}${id}_start.time.log
    $client > ${type}${id}.out 2>&1
    ts > ${type}${id}_end.time.log
}

# Run all clients in parallel
run_client 1 time & run_client 2 time & run_client 3 time &
run_client 1 nbproc & run_client 2 nbproc & run_client 3 nbproc &
run_client 1 sysinfo & run_client 2 sysinfo & run_client 3 sysinfo &

wait
pkill -P $SERVER_PID