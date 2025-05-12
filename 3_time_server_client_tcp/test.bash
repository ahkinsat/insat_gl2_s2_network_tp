#!/bin/bash

# Configuration
PORT=8080
#TSHARK_OUT="tmp.pcap"
TSHARK_OUT="delay-tmp.pcap"
#SERVER_OUT="server.out"
SERVER_OUT="delay-server.out"
#CLIENT_OUT="client.out"
CLIENT_OUT="delay-client.out"
SERVER_BIN="./server"
CLIENT_BIN="./client"

# Cleanup
rm -f "$TSHARK_OUT" "$SERVER_OUT" "$CLIENT_OUT"

# Start tshark in background (lo interface, port 8080, TCP only)
tshark -i lo -w "$TSHARK_OUT" -s 0 -f "tcp port $PORT" &
TSHARK_PID=$!
sleep 1  # delay

# Start server (redirect output)
"$SERVER_BIN" > "$SERVER_OUT" 2>&1 &
SERVER_PID=$!
sleep 0.5  # delay to bind

# Start client (redirect output)
"$CLIENT_BIN" > "$CLIENT_OUT" 2>&1


sleep 1  
kill -INT "$TSHARK_PID"  # stop tshark ^C
kill "$SERVER_PID"       # Stop server

# Analyze results
echo -e "\n=== Results ==="
echo "Server output:"
cat "$SERVER_OUT"

echo -e "\nClient output:"
cat "$CLIENT_OUT"

echo -e "\nPacket analysis:"
tshark -r "$TSHARK_OUT" -qz "io,stat,0,tcp.analysis.retransmission"  # Retransmissions
tshark -r "$TSHARK_OUT" -Y "tcp" # -Tfields -e frame.number -e tcp.srcport -e tcp.dstport -e tcp.len