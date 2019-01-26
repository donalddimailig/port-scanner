**Port Scanner Implementation**

TCP port scanners work by establishing connection to the ports of a host to be scanned. If a connection is established then the port is open. This port scanner implementation uses sockets and runs a loop to connect to the ports of the target host.
B. How To Use

Since the implementation uses threads, to compile the code: **gcc port-scanner.c -lpthread**

By default, if no port or range of ports is specified, the scanner will scan the first 1000 ports (port 1-1000) of the target host. The code can also scan for open ports of a range of IP addresses and even a /24 network.

**Additional step** (because there will be too many open sockets): Command: *ulimit -n 1000000* (tested on Ubuntu 16.04)
