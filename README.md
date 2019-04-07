# tun-tap-explore

This is an example C++ project to demonstrate `tun interface`.

## Architecture
![alt text](https://github.com/ddeka0/tun-tap-explore/blob/ddeka0-patch-1/tun-tap%20(3).png)

Machine `A` has a payload `Hellow from client`. It writes this payload to a `UDP` client socket. Destination of this UDP packet is `C`. Client does this continuously in a different thread.

The client also creates a tun interface (`tun1`). This is done in the main thread. A routing rule is added in the machine `A` to send IP packet destined to `C` towards `tun1` interface. Due to this setup, IP packets can not go out of machine A directly (although C is reachable from A, A and C are in the same network in my setup).

The main thread in the client captures (reads continuously) the packets from the `tun1` interface and write to a UDP socket again destined to machine `B`. 

Kernel at machine `B` will reaceive a UDP packet with a payload which is again a IP packet. Kernel removes the outer IP header and we receive the payload in the application layer. This payload is an IP packet(with destination address `C`). We want to forward to machine `C`. But this can not be written to a normal socket destined to machine `C`, because it will add one more IP header. We dont want any more header. 

Therefore server running at machine B also creates one tun interface (`tun2`). It writes the payload in the `tun file descriptor`. 

We need to enable packet forwarding and disbale `Reverse path filtering (rp_filter)` in the machine `B`.

          `sysctl -w net.ipv4.ip_forward=1`
          `sysctl -w net.ipv4.conf.tun2.rp_filter=0`
          `sysctl -w net.ipv4.conf.all.rp_filter=0`

Routing table in machine B: (I have not added any more static routes)
          
          Kernel IP routing table
          Destination     Gateway         Genmask         Flags Metric Ref    Use Iface
          default         _gateway        0.0.0.0         UG    100    0        0 eth0
          10.129.131.0    0.0.0.0         255.255.255.0   U     0      0        0 eth0
          _gateway        0.0.0.0         255.255.255.255 UH    100    0        0 eth0

## Instruction to run the project

Three virtual machines, containers or physical machines are required. Assuming IPs are allocated already and they are in the same network.

Machine A:
          
          g++ client.cpp -pthread
          sudo ./a.out
          
          sudo ifconfig tun1 up
          sudo route add -host 10.129.131.220 tun1
Machine B:

          g++ middle-server.cpp
          sudo ./a.out
          
          sudo ifconfig tun2 up
          sysctl -w net.ipv4.ip_forward=1
          sysctl -w net.ipv4.conf.tun2.rp_filter=0
          sysctl -w net.ipv4.conf.all.rp_filter=0
Machine C:
          
          g++ sink.cpp
          ./a.out
          
          
## Authors

* **Debashish Deka** 
