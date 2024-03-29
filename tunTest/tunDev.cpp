#include "tunDev.h"
int tundev::createDevice(int flags) {
    char tunNameBuf[IFNAMSIZ];
    devName.copy(tunNameBuf,devName.length());
    tunNameBuf[devName.length()] = 0;
    const char *clonedev = "/dev/net/tun";
    if((fd = open(clonedev, O_RDWR)) < 0 ) {
        std::cout <<"Open tunfd failed "<<strerror(errno)<< std::endl;
        return fd;
    }
    memset(&ifr, 0, sizeof(ifr));
    /* Flags:   IFF_TUN   - TUN device(no Ethernet headers) 
    *           IFF_TAP   - TAP device
    *           IFF_NO_PI - Do not provide packet information
    *           (https://github.com/kohler/click/pull/397)  
    */
    ifr.ifr_flags = flags;
    /*if devName was provided by user, copy it to the ifr struct */
    if (this->devName.length() != 0) {
        strncpy(ifr.ifr_name, tunNameBuf, IFNAMSIZ);
    }
    /* TODO explain TUNSETIFF */
    if((err = ioctl(this->fd, TUNSETIFF, (void *) &ifr)) < 0 ) {
        perror("ioctl");
        close(fd);
        return err;
    }
    /* overwrite the allocated name */
    devName = std::string(ifr.ifr_name);
    return fd;       
}
int tundev::receiveData(char *buf, int count) {
    int rlen;
    rlen = read(this->fd, buf, count);
    if (rlen < 0) {
        std::cout <<"receiveData has failed "<<strerror(errno) << std::endl;
        return errno;
    }
    return rlen;
}
int tundev::sendData(char *buf, int count) {
    int wlen;
    wlen = write(this->fd, buf, count);
    if (wlen < 0) {
        std::cout <<"sendData has failed "<<strerror(errno) << std::endl;
        return errno;
    }
    return wlen;
}
int tundev::up() {
    set_if_up(this->devName,1);
}
int tundev::down() {
    set_if_down(this->devName,1);
}
int tundev::isUp() {
    return check_if_is_up(this->devName);
}