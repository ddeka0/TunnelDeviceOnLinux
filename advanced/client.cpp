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

using namespace std;

#define PORT     		8080					// both DN and UPF listens at the same port
#define MAXLINE 		1024
#define SINK_SERVER_IP		"10.129.131.194"
#define MIDDLE_SERVER_IP	"10.129.131.213"
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

void upf_rcv_function(int sockfd,int tunfd) {
	char buf[MAXLINE];
	int len;
	struct sockaddr_in	cliaddr;
	long long int cnt = 0;	
	while(true) {
		int n = recvfrom(sockfd, (char *)buf, MAXLINE,
				MSG_WAITALL, ( struct sockaddr *) &cliaddr,
				(socklen_t*)&len);
		cout << BOLDMAGENTA <<"RAN : ["<<cnt++<<"] :: Received form UPF(udp socket) of "<< n <<" bytes" << RESET << endl;
		send_data(tunfd,buf,MAXLINE);
		cout << BOLDMAGENTA <<"RAN : "<<"Writing into tun2 device for [br0:2] virtual interface" << RESET << endl;
		memset(buf,0,sizeof(buf));
	}
}

void ue_rcv_function(int sockfd) {
	char buf[MAXLINE];
	int len;
	struct sockaddr_in cliaddr;
	long long int cnt = 0;
	while(true) {
		int n = recvfrom(sockfd, (char *)buf, MAXLINE,
				MSG_WAITALL, ( struct sockaddr *) &cliaddr,
				(socklen_t*)&len);
		cout << BOLDRED <<"UE : "<<"[message from DN] "<<buf << RESET << endl;
		cout << BOLDRED <<"UE : ["<<cnt++<<"]"<<" Received form Data Network(br0:2 interface) "<< n <<" bytes" << RESET << endl;
		memset(buf,0,sizeof(buf));
	}
}

int main() {

   	char tun_name[IFNAMSIZ];
   	strcpy(tun_name, "tun1");
	int tunfd = tun_alloc(tun_name, IFF_TUN | IFF_NO_PI);

	char tun_name2[IFNAMSIZ];
	strcpy(tun_name2, "tun2");
	int tunfd2 = tun_alloc(tun_name2, IFF_TUN | IFF_NO_PI);

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
			cout << BOLDCYAN <<"UE : ["<< msg << "] :: message sent to Data Network"<< RESET << endl;
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

	memset(&servaddr, 0, sizeof(servaddr));

	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);
	servaddr.sin_addr.s_addr = inet_addr(MIDDLE_SERVER_IP);

	std::thread upf_rcv_thread(upf_rcv_function,sockfd,tunfd2);
	upf_rcv_thread.detach();


	long long int cnt = 0;
	
	while(true) {
		char buf[MAXLINE];
		int len = receive_data(tunfd,buf,MAXLINE);
		cout << BOLDYELLOW <<"RAN : "<<"Number of bytes captured (using tun1) = " << len << RESET <<endl;
		// sending to UPF;
		sendto(sockfd, buf, len,
			MSG_CONFIRM, (const struct sockaddr *) &servaddr,
			sizeof(servaddr));
		cout << BOLDYELLOW <<"RAN : "<<"Packet sent by RAN to UPF" << RESET << endl;
		memset(buf,0,sizeof(buf));
		cnt++;
	}	

	return 0;
}
