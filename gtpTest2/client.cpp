/*
sudo ifconfig br0:2 172.112.100.2 netmask 255.255.255.0

sudo ifconfig tun1 up
sudo ifconfig tun2 up
sudo route add -host 10.129.131.194 tun1
sudo route add -host 172.112.100.2 br0:2

sudo sysctl -w net.ipv4.conf.tun2.rp_filter=0
sudo sysctl -w net.ipv4.conf.all.rp_filter=0
sudo sysctl -w net.ipv4.ip_forward=1

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

#include "gtpMessage.h"

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
    return wlen;
}
void printArray(char * buf, int bufsize) {
	for(int i=0; i<bufsize; i++)
		printf("0x%02X ", buf[i]);

	printf("\n");
}

void upf_rcv_function(int sockfd,int tunfd) {
	char buf[MAXLINE];
	int len;
	struct sockaddr_in	cliaddr;
	long long int cnt = 0;	
	while(true) {
		int n = recvfrom(sockfd, (char *)buf, MAXLINE,
				MSG_WAITALL, ( struct sockaddr *) &cliaddr,
				(socklen_t*)&len);
		cout << BOLDMAGENTA 
			<<"RAN : ["<<cnt++<<"] :: Received form UPF(udp socket) of "<< n 
			<<" bytes" << RESET << endl;
		/*	UPF received a downlink packet
		*	Decaptulate the received packet 
		* 	take out the UE targeted IP packet and write it to tun device	
		*/
		gtpMessage gtpMsg;
		if(decodeGtpMessage((uint8_t*)buf,&gtpMsg,n) == FAILURE) {
			cout << BOLDMAGENTA << "RAN : decodeGtpMessage() failed"
				<< RESET << endl;
			continue; 
		}
		/*Process the GTP Header if required*/
		// write tp TUN device so that UE can read it from virtual interface
		send_data(tunfd,(char *)(gtpMsg.payload),gtpMsg.payloadLength);
		cout << BOLDMAGENTA <<"RAN : "
			<<"Writing into tun2 device for [br0:2] virtual interface" 
			<< RESET << endl;
		memset(buf,0,sizeof(buf));
	}
}

void ue_rcv_function(int sockfd) {
	char buf[MAXLINE];
	int len;
	struct sockaddr_in cliaddr;
	long long int cnt = 0;
	while(true) {
		cout << "rcv data from DN";
		int n = recvfrom(sockfd, (char *)buf, MAXLINE,
				MSG_WAITALL, ( struct sockaddr *) &cliaddr,
				(socklen_t*)&len);
		cout << BOLDRED <<"UE : "<<"[message from DN] "<<buf << RESET << endl;
		cout << BOLDRED <<"UE : ["<<cnt++<<"]"
			<<" Received form Data Network(br0:2 interface) "
			<< n <<" bytes" << RESET << endl;
		memset(buf,0,sizeof(buf));
	}
}

int main() {

   	char tun_name[IFNAMSIZ];
   	strcpy(tun_name, "tun1");
	int tunfd = tun_alloc(tun_name, IFF_TUN | IFF_NO_PI);
	/*	NOTE: UE will write to a socket with a destination IP address = X
	*	We have added a routing rule 
	*	sudo route add -host X tun1, packet will come out of tun1
	*	RAN reads from tun1 fd
	*	
	*	In the reverse path, We write to the tun1 fd and 
	*	with the help of "sudo route add -host 172.112.100.2 enp0s31f6:2"
	*	UE can read the packets from its socket
	*/
	getchar();

	std::thread ue_thread([] {

		int sockfd;
		char buffer[MAXLINE];
		struct sockaddr_in     servaddr,cliaddr;
		
		if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
			perror("socket creation failed");
			exit(EXIT_FAILURE);
		}

		// bind the client (UE) to an virtual interface
		struct sockaddr_in localaddr;
		localaddr.sin_family = AF_INET;
		localaddr.sin_addr.s_addr = inet_addr(LOCAL_CLIENT_IP);
		localaddr.sin_port = 0;  // Any local port will do
		bind(sockfd, (struct sockaddr *)&localaddr, sizeof(localaddr));

		memset(&servaddr, 0, sizeof(servaddr));

		servaddr.sin_family = AF_INET;
		servaddr.sin_port = htons(PORT);
		servaddr.sin_addr.s_addr = inet_addr(SINK_SERVER_IP);

		int n, len;
		long long cnt = 0;
		string hello = " Hello from client";
		string msg = to_string(cnt) + hello;


		std::thread ue_rcv_thread(ue_rcv_function,sockfd);
		ue_rcv_thread.detach();

		while(true) {
			sleep(1); // wait for 1 sec
			sendto(sockfd, (msg.c_str()), msg.length(),
				MSG_CONFIRM, (const struct sockaddr *) &servaddr,
				sizeof(servaddr));
			cout << BOLDCYAN 
				<<"UE : ["<< msg << "] :: message sent to Data Network"
				<< RESET << endl;
			cnt++;
			string prefix = to_string(cnt);
			msg = prefix + hello;
		}
		close(sockfd);
	
	});
	ue_thread.detach();
    

	// create a client-socket to send captured IP packects 
	int sockfd;
	char buffer[MAXLINE];
	struct sockaddr_in     servaddr,cliaddr;

	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}

	// BIND CLIENT TEMP FIX ... 
	// bind the client (UE) to an virtual interface
	struct sockaddr_in localaddr;
	localaddr.sin_family = AF_INET;
	localaddr.sin_addr.s_addr = inet_addr("10.129.131.157");
	localaddr.sin_port = htons(2152);
	bind(sockfd, (struct sockaddr *)&localaddr, sizeof(localaddr));




	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT_UPF);
	servaddr.sin_addr.s_addr = inet_addr(MIDDLE_SERVER_IP);

	std::thread upf_rcv_thread(upf_rcv_function,sockfd,tunfd);	// tunfd2
	upf_rcv_thread.detach();


	uint16_t cnt = 0;
	
	while(true) {
		char ipPayload[MAXLINE];
		/*read an UE pakcet from the tun device*/
		int payLoadLen = receive_data(tunfd,ipPayload,MAXLINE);
		cout << BOLDYELLOW <<"RAN : "<<"Number of bytes captured (using tun1)= "
			<< payLoadLen << RESET <<endl;
		
	    // Encapsulate this UE packet inside a GTP header 
		gtpMessage gtpMsg;
        /*copy the payload first */
        gtpMsg.payloadLength = payLoadLen/* strlen(ipPayload) */;
        memcpy(&gtpMsg.payload,ipPayload,payLoadLen/* strlen(ipPayload) */);
        //printArray((char *)gtpMsg.payload, payLoadLen);
		
		/*fill the header */
        gtpMsg.gtp_header.flags = 0b00110010;
        gtpMsg.gtp_header.msgType = 0xFF;
        gtpMsg.gtp_header.length = payLoadLen; // TODO fix the len later
		if(gtpMsg.gtp_header.flags & (GTP_S_MASK|GTP_PN_MASK|GTP_E_MASK))
		{
			// update length field
		 	gtpMsg.gtp_header.length += GTP_HDR_OPTIONAL_FIELD_LENGTH;
		}
        gtpMsg.gtp_header.teid = 101;	// TODO fix

		// set optional fields
		
		
		gtpMsg.gtp_header.seqNo = cnt;
		
        uint8_t buffer[MAXLINE];
        uint32_t encodedLen = 0;
        memset(buffer,0,sizeof(buffer));
        if(encodeGtpMessage(buffer,MAXLINE,&gtpMsg,&encodedLen) == FAILURE) {
            cout <<"encodeGtpMessage failed"<<endl;
            return 0;
        }
		// Encapsulation is done, Now send this packer over UDP to UPF

		sendto(sockfd, buffer, encodedLen,
			MSG_CONFIRM, (const struct sockaddr *) &servaddr,
			sizeof(servaddr));
		cout << BOLDYELLOW <<"RAN : "<<"Packet sent by RAN to UPF" 
			<< RESET << endl;
		
		memset(buffer,0,sizeof(buffer));
		cnt++;
	}	

	return 0;
}
