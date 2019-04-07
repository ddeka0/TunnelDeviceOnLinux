// sudo ifconfig tun1 up
// sudo route add -host 10.129.131.171 tun1
// This is 
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

#define PORT            8080    // both DN and UPF listens at the same port
#define MAXLINE         1024
#define MIDDLE_SERVER_IP   "10.129.131.213"

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

int tun_alloc(char *dev, int flags) {
    struct ifreq ifr;
    int fd, err;
    const char *clonedev = "/dev/net/tun";
    if( (fd = open(clonedev, O_RDWR)) < 0 ) {
        cout << "something wrong !" << endl;
        return fd;
    }
    memset(&ifr, 0, sizeof(ifr));
    ifr.ifr_flags = flags;
    if (*dev) {
        strncpy(ifr.ifr_name, dev, IFNAMSIZ);
    }
    if( (err = ioctl(fd, TUNSETIFF, (void *) &ifr)) < 0 ) {
        perror("ioctl");
        close(fd);
        return err;
    }
    strcpy(dev, ifr.ifr_name);
    return fd;
}
int receive_data(int tunfd, char *packet, int bufsize) {
    int len;
    len = read(tunfd, packet, bufsize);
    if (len < 0) {
        perror("read");
    }
    return len;
}
int send_data(int tunfd, char *packet, int len) {
    int wlen;
    wlen = write(tunfd, packet, len);
    if (wlen < 0) {
        perror("write");
    }
    cout << MAGENTA << wlen <<" bytes written to tun1 device "<< RESET << endl;
    return wlen;
}
void printArray(char * buf, int bufsize) {
    for(int i=0; i<bufsize; i++)
        printf("0x%02X ", buf[i]);

    printf("\n");
}
// Driver code 
int main() { 
    

    char tun_name[IFNAMSIZ];
    strcpy(tun_name, "tun2");
    int tunfd = tun_alloc(tun_name, IFF_TUN | IFF_NO_PI);

    // create an UDP server socket
    // UPF will read packets from the RAN side
    // and supposed to remove the GTP header
    // and then write to a tun device again

    int sockfd; 
    char buffer[MAXLINE];  
    struct sockaddr_in servaddr, cliaddr; 
     
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
        perror("socket creation failed"); 
        exit(EXIT_FAILURE); 
    } 
    
    memset(&servaddr, 0, sizeof(servaddr)); 
    memset(&cliaddr, 0, sizeof(cliaddr)); 
    
    servaddr.sin_family = AF_INET; // IPv4 
    servaddr.sin_addr.s_addr = inet_addr(MIDDLE_SERVER_IP);
    servaddr.sin_port = htons(PORT);
    
    // Bind the socket with the server address 
    if ( bind(sockfd, (const struct sockaddr *)&servaddr, 
            sizeof(servaddr)) < 0 ) 
    { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    } 

    while(1) {
        int len, n;
        cout <<"reading ..." << endl; 
        n = recvfrom(sockfd, (char *)buffer, MAXLINE, 
            MSG_WAITALL, ( struct sockaddr *) &cliaddr, 
            (socklen_t*)&len); 
        buffer[n] = '\0'; 
        cout <<"Number of bytes read by UPF = "<< n << endl;
        cout <<"writing to tun interface again towards DN network" << endl;
        send_data(tunfd,buffer,n);
        memset(buffer,0,sizeof(buffer));
    }    
     
    return 0;
} 

