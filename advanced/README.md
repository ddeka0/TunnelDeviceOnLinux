# tun-tap-explore/advanced

This is an example C++ project to demonstrate `tun interface`.

## Client Machine:
          Generates UDP traffic with destination address `10.129.131.194` and sends. A `tun` device is set up in the client side to capture packets with destination address `10.129.131.194`. Following two commands are required to active the tun device and setup the route.
          
          sudo ifconfig tun1 up
          sudo route add -host 10.129.131.194 tun1

 
          
          
          
## Authors

* **Debashish Deka** 
