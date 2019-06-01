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

using namespace std;

#define PORT     		8080		// both DN and UPF listens at the same port
#define PORT_UPF     	2152
#define MAXLINE 		1024
#define SINK_SERVER_IP		"10.129.131.180"
#define MIDDLE_SERVER_IP	"10.129.131.206"
#define LOCAL_CLIENT_IP		"172.112.100.2"		// this is virtual interface

//the following are UBUNTU/LINUX, and MacOS ONLY terminal color codes.
#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */
#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */

#define FAILURE 	-1
#define SUCCESS		0


#include "tunDev.h"
#include "intfUtils.h"
#include <netinet/ip.h>
int main() {
    /* Create one tun device */
    tundev tun;
    tun.devName = "tunA";
    if(tun.createDevice() < 0) {
        std::cout <<"Program Exiting.."<<std::endl;      
    }

    tun.up();

    getchar();

    tun.down();

    getchar();

    tun.up();


    char buf[1024];
    while(1) {
        int n = tun.receiveData(buf,1024);

        struct iphdr *header = (struct iphdr *)(buf);

        tun.sendData(buf,n);
    }
    getchar();

}