#include "intfUtils.h"
/* Ref:
https://elixir.bootlin.com/linux/v3.10/source/Documentation/networking/ifenslave.c#L990
*/
int set_if_flags(char *ifname, short flags) {
	struct ifreq ifr;
	int res = 0;
	ifr.ifr_flags = flags;
    strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
    if ((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        printf("socket error %s\n", strerror(errno));
        res = -1;
        return res;
    }
    res = ioctl(skfd, SIOCSIFFLAGS, &ifr);
    if (res < 0) {
		printf("Interface '%s': Error: SIOCSIFFLAGS failed: %s\n",
			ifname, strerror(errno));
	} else {
		printf("Interface '%s': flags set to %04X.\n", ifname, flags);
	}
    close(skfd);
	return res;
}
int set_if_up(std::string intfName, short flags) {
	char ifname[IFNAMSIZ];
    strcpy(ifname, intfName.c_str());
    return set_if_flags(ifname, flags | IFF_UP);
}
int set_if_down(std::string intfName, short flags) {
	char ifname[IFNAMSIZ];
    strcpy(ifname, intfName.c_str());
	return set_if_flags(ifname, flags & ~IFF_UP);
}
int check_if_is_up(std::string intfName) {
    struct ifreq ifr;
    if ((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        printf("socket error %s\n", strerror(errno));
        return -1;
    }
    memset(&ifr, 0, sizeof(ifr));
    strcpy(ifr.ifr_name, intfName.c_str());
    if (ioctl(skfd, SIOCGIFFLAGS, &ifr) < 0) {
        // perror("SIOCGIFFLAGS");
        printf("ioctl error %s\n", strerror(errno));
        return -1;
    }
    close(skfd);
    return (ifr.ifr_flags & IFF_UP);
}