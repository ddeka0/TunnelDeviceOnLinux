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

#include "gtpMessage.h"

#define MAXLINE     1024
#define PORT1       8080
#define PORT        2152
#define FAILURE     -1
#define SUCCESS     0
#define DNN_SERVER_IP       "10.129.131.200"
#define MIDDLE_SERVER_IP    "10.129.131.157"
using namespace std;



// sudo ifconfig tun1 up
// sudo route add -host 10.129.131.171 tun1



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
void printArray(char * buf, int bufsize) {
    for(int i=0; i<bufsize; i++)
        printf("0x%02X ", buf[i]);

    printf("\n");
}
int main() {

    char tun_name[IFNAMSIZ];
    strcpy(tun_name, "tun1");
    int tunfd = tun_alloc(tun_name, IFF_TUN | IFF_NO_PI);

    getchar();

	std::thread t([] {
			int sockfd;
			struct sockaddr_in     servaddr;
			if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
				perror("socket creation failed");
				exit(EXIT_FAILURE);
			}

			memset(&servaddr, 0, sizeof(servaddr));

			servaddr.sin_family = AF_INET;
			servaddr.sin_port = htons(PORT1);
			servaddr.sin_addr.s_addr = inet_addr(DNN_SERVER_IP);

            char str[] = "Hello";

			while(1) {
				sleep(1);
				sendto(sockfd, str, strlen(str),
					MSG_CONFIRM, (const struct sockaddr *) &servaddr,
					sizeof(servaddr));
				cout << GREEN << str << " :: message sent by UE\n" << RESET << endl;
			}
			close(sockfd);
	});

	t.detach();
    
    

    // create a client-socket to send captured IP packects 
	int sockfd;
	struct sockaddr_in     servaddr;

	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}

	memset(&servaddr, 0, sizeof(servaddr));

	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);
	servaddr.sin_addr.s_addr = inet_addr(MIDDLE_SERVER_IP);    
    

	long long int cnt = 0;
	while(1) {
		cout << cnt << endl;
		char ipPayload[MAXLINE];
		cout <<"reading..." << endl;
		int len = receive_data(tunfd,ipPayload,MAXLINE);

        gtpMessage gtpMsg;
        /*copy the payload first */
        gtpMsg.payloadLength = len/* strlen(ipPayload) */;
        memcpy(&gtpMsg.payload,ipPayload,len/* strlen(ipPayload) */);
        /*fill the header */
        gtpMsg.gtp_header.flags = 0b00110000;
        gtpMsg.gtp_header.msgType = 0xFF;
        gtpMsg.gtp_header.length = len; // TODO change length later
		/* 	header.length = optional_field.length + payloadLength
			len only has payloadLength	
		*/
        gtpMsg.gtp_header.teid = 101;


        uint8_t buffer[MAXLINE];
        uint32_t encodedLen = 0;
        memset(buffer,0,sizeof(buffer));
        if(encodeGtpMessage(buffer,MAXLINE,&gtpMsg,&encodedLen) == FAILURE)
        {
            cout <<"encodeGtpMessage failed"<<endl;
            return FAILURE;
        }


		cout << YELLOW <<"Number of bytes captured by RAN = " << len << RESET <<endl;
        printf("buffer val: %2x", buffer);
        printf("buffer val2: %s", buffer);
		// printArray(buf, len);
		sendto(sockfd, buffer, encodedLen,
			MSG_CONFIRM, (const struct sockaddr *) &servaddr,
			sizeof(servaddr));
		cout << YELLOW << "Packet sent by RAN to UPF" << RESET << endl;
		cnt++;
	}	

	return 0;
}
