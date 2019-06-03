#include "addRoute.h"

/* add other arguments and filters if needed */
int configRoute(std::string intfName,std::string dstIp,int flag) {
    // create the control socket.
    int fd = socket( PF_INET, SOCK_DGRAM, IPPROTO_IP );
    if(fd < 0) {
        std::cout <<"socket creation failed"<< strerror(errno) << std::endl;
        return -1;
    }
    struct rtentry route;
    memset(&route,0,sizeof(route));
    // set the gateway to 0.
    struct sockaddr_in *addr = (struct sockaddr_in *)&route.rt_gateway;
    addr->sin_family = AF_INET;
    addr->sin_addr.s_addr = 0;

    // set the host we are configuring. 
    addr = (struct sockaddr_in*) &route.rt_dst;
    addr->sin_family = AF_INET;
    addr->sin_addr.s_addr = inet_addr(dstIp.c_str());

    // Set the mask. In this case we are using 255.255.255.255, to block a single
    // IP. But you could use a less restrictive mask to block a range of IPs. 
    // To block and entire C block you would use 255.255.255.0, or 0x00FFFFFFF
    addr = (struct sockaddr_in*) &route.rt_genmask;
    addr->sin_family = AF_INET;
    addr->sin_addr.s_addr = inet_addr(GEN_MASK);

    // These flags mean: this route is created "up", or active
    // The blocked entity is a "host" as opposed to a "gateway"
    // The packets should be rejected. On BSD there is a flag RTF_BLACKHOLE
    // that causes packets to be dropped silently. We would use that if Linux
    // had it. RTF_REJECT will cause the network interface to signal that the 
    // packets are being actively rejected.
    route.rt_flags = RTF_UP | RTF_HOST;
    route.rt_metric = 0;

    char ifname[intfName.length() + 1];
    strncpy(ifname,intfName.c_str(),intfName.length() + 1);
    route.rt_dev = ifname;

    // add route cia SIOCADDRT call
    // delete route via SIOCDELRT call
    uint32_t mask = (flag == ADD_ROUTE) ? SIOCADDRT : SIOCDELRT;
    int ret = ioctl( fd, mask, &route );
    if (ret < 0) {
        std::cout <<"ioctl error."<< strerror(errno) << std::endl;
        close(fd);
        return -1;
    }
    // remember to close the socket lest you leak handles.
    close( fd );
    return 0; 
}

/*int main()
{
    while(1)
    {
        if (!configRoute("enp0s31f6","10.129.131.180",ADD_ROUTE))
        {
            printf("route added successfully\n");
            break;
        }
        else
        {
            printf("failed to add route\n");
            configRoute("enp0s31f6","10.129.131.180",DEL_ROUTE);
            printf("Route deleted\n");
            break;
        }
    }
    
    return 0;
}*/