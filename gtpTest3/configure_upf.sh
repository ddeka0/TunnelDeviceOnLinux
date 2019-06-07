sudo ifconfig tun2 up
sudo sysctl -w net.ipv4.conf.tun2.rp_filter=0
sudo sysctl -w net.ipv4.conf.all.rp_filter=0
sudo sysctl -w net.ipv4.ip_forward=1
sudo route add -host 172.112.100.2 tun2 # was previously tun3
