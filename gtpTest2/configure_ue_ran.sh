sudo ifconfig tun1 up
sudo ifconfig tun2 up
sudo route add -host 10.129.131.180 tun1	#route uplink packets
sudo route add -host 172.112.100.2 enp0s31f6:2	#route downllink packets

sudo sysctl -w net.ipv4.conf.tun2.rp_filter=0
sudo sysctl -w net.ipv4.conf.all.rp_filter=0
sudo sysctl -w net.ipv4.ip_forward=1
