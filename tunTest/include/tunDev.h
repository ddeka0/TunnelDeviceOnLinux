#pragma once
/*This header files contains the declaration of the following utility functions
*   1. createTunDev()
*   2. upIntf()
*   3. downIntf()
*   4. routeAdd()
*   5. delRoute()
*   6. isIntfUp()
*   7. sysctl calls // not here. check this
*/
#include <bits/stdc++.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/if.h>
#include <linux/if_tun.h>

#include "intfUtils.h"

#define SUCCESS 0
#define FAILURE (-1)

class tundev {
public:
    int fd;
    int interfaceFd;
    int err;
    std::string devName;
    std::string ipAddr;
    struct ifreq ifr;
    tundev() {
        this->devName = std::string();
        this->ipAddr = std::string();
    }
    /* createDevice() creates a tun device and sets fd
    * usage: 
    * tundev tun;
    * if(tun.createDevice("tunA") < 0) {
    *   // handle error
    * }
    * then use tun.fd
    # Or,
    * if(tun.createDevice("tunA",IFF_TUN | IFF_NO_PI) < 0) {
    *   // handle error
    * }
    * then use tun.fd
    */
    int createDevice(int flags = IFF_TUN | IFF_NO_PI);
    /* 
    * allocateIpToTunDevice() allocates a specified IP to TUN device
    */
    int allocateIpToTunDevice(std::string);
    /* 
    * bindEgressTrafficToTunDevice() bind outgoing traffic to TUN interface
    */
    int bindEgressTrafficToTunDevice();
    /* receiveData() attempts to read up to count bytes from file descriptor 
    * of this class (this->fd) into the buffer starting at buf
    */
    int receiveData(char *buf, int count);
    /* sendData() writes up to count bytes from the buffer \
    * starting at buf to the file referred to by the file descrip‐tor
    * of this class (this->fd)
    */
    int sendData(char *buf, int count);

    /* activate this tun device */
    int up();

    /* deactivates this tun device */
    int down();

    int isUp();

};
