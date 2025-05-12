import socket
import threading
import time
import sys
from statistics import mean

# Dictionary to store results from all threads
results = {
    'per_client': [],
    'all_mins': [],
    'all_maxs': [],
    'all_avgs': []
}

def client_thread(client_id):
    timestamps = []
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect(('localhost', 8080))
    s.send(b'Hello')
    
    for _ in range(60):
        data = s.recv(1024)
        timestamps.append(time.time())
    
    s.close()
    
    # Calculate intervals between messages
    intervals = [timestamps[i+1]-timestamps[i] for i in range(len(timestamps)-1)]
    min_delay = min(intervals)
    max_delay = max(intervals)
    avg_delay = mean(intervals)
    
    # Store results
    with threading.Lock():
        results['per_client'].append((client_id, min_delay, max_delay, avg_delay))
        results['all_mins'].append(min_delay)
        results['all_maxs'].append(max_delay)
        results['all_avgs'].append(avg_delay)

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python test.py <num_clients>")
        sys.exit(1)
    
    N = int(sys.argv[1])
    threads = []
    
    print(f"Starting test with {N} clients...")
    start_time = time.time()
    
    # Create and start client threads
    for i in range(N):
        t = threading.Thread(target=client_thread, args=(i,))
        threads.append(t)
        t.start()
    
    # Wait for all threads to complete
    for t in threads:
        t.join()
    
    test_duration = time.time() - start_time
    print(f"\nTest completed in {test_duration:.2f} seconds")
    
    # Print per-client results
    print("\nPer-client results:")
    for client_id, min_d, max_d, avg_d in results['per_client']:
        print(f"[Client {client_id:2d}] min: {min_d:.6f}s, max: {max_d:.6f}s, avg: {avg_d:.6f}s")
    
    # Print aggregated statistics
    print("\nAggregated statistics across all clients:")
    print(f"Minimum delays: min={min(results['all_mins']):.6f}s, avg={mean(results['all_mins']):.6f}s, max={max(results['all_mins']):.6f}s")
    print(f"Maximum delays: min={min(results['all_maxs']):.6f}s, avg={mean(results['all_maxs']):.6f}s, max={max(results['all_maxs']):.6f}s")
    print(f"Average delays: min={min(results['all_avgs']):.6f}s, avg={mean(results['all_avgs']):.6f}s, max={max(results['all_avgs']):.6f}s")