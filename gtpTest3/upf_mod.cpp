/*

ifconfig tun2 up
ifconfig tun3 up
sysctl -w net.ipv4.conf.tun2.rp_filter=0
sysctl -w net.ipv4.conf.all.rp_filter=0
sysctl -w net.ipv4.conf.tun3.rp_filter=0
sysctl -w net.ipv4.ip_forward=1
sudo route add -host 172.112.100.2 tun3

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
#include <tunDev.h>
#include <intfUtils.h>
#include <netinet/ip.h>
#include <addRoute.h>
#include <setupRules.h>


using namespace std;

#define PORT            2152    // both DN and UPF listens at the same port
#define MAXLINE         1024
#define UPF_SERVER_IP   "10.129.131.206"
#define RAN_IP			"10.129.131.157"
#define UE_IP 			"172.112.100.2"
#define SUCCESS 0
#define FAILURE (-1)

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


int decapsulateGtpMessage(char *,int, char *, uint32_t *);
int encapsulateGtpMessage(char *,int, char *, uint32_t *); 

struct sockaddr_in ranaddr;
uint16_t globalSeqNum = 0;


void printArray(char * buf, int bufsize) {
	for(int i=0; i<bufsize; i++)
		printf("0x%02X ", buf[i]);

	printf("\n");
}

void thread_func(tundev tun,int sockfd) {
	char innerPacket[MAXLINE];
	char buffer[MAXLINE];
	uint32_t encapLength;
	long long int cnt = 0;

	while(1) {

		/* Route packet with destination <UE_IP> to tun device
		* 		sudo route add -host 172.112.100.2 tun2
		* As of now, using commandline, TODO: do it from code
		*/

		/* Receiving data from server via tun device */
		int len = tun.receiveData(innerPacket,MAXLINE);
		// cout << BOLDGREEN <<" UPF: "<<cnt<<" :: Number of downlink bytes"
		// 		"captured by UPF (using tun dev) = " << len << RESET <<endl;
		
		/* 
		*  Adding GTP header over inner packet received from server
		*  and serializing(encoding) the packet
		*/
		if(encapsulateGtpMessage(innerPacket, len, buffer, 
				&encapLength) == FAILURE)
		{
			//cout<<"UPF: Packet encapsulation failed. Aborting!\n";
			return;
		}
		
		/* Sending packet over GTP tunnel to RAN  (UDP over GTP) */
		sendto(sockfd, buffer, encapLength,
				MSG_CONFIRM, (const struct sockaddr *) &ranaddr,
				sizeof(ranaddr));
		// cout << BOLDGREEN << "UPF: Dowlink packet sent by UPF to RAN" 
		// 		<< RESET << endl;
		cnt++;
		memset(buffer,0,sizeof(buffer));
	}
}
int main() {

	int sockfd;
	tundev tun1;
	tun1.devName = "tunUPF0";
	if(tun1.createDevice() < 0) {
        //printf("Error! Cannot create tun device. Abort\n");
		return 0;
    }
	tun1.up();
	if(tun1.isUp())
	{
		// setup route
		if(configRoute(tun1.devName, UE_IP, ADD_ROUTE) == SUCCESS)
		{
			//printf("Route added successfully\n");
		}
		else
		{
			//printf("Failed to add route\n");
		}
		
		// add forward and rp_filter rules
		setIpForwardRules(FORWARD_ENABLE);
		setRpFilterRules("all", RP_FILTER_DISABLE);
		setRpFilterRules(tun1.devName, RP_FILTER_DISABLE);
	}
	else
	{
		//printf("TUN device is not up. Abort!\n");
		return 0;
	}

	// create an UDP server socket
	// UPF will read packets from the RAN side
	// and supposed to remove the GTP header
	// and then write to a tun device again

	char buffer[MAXLINE];  
	struct sockaddr_in servaddr, cliaddr; 
	 
	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
		perror("socket creation failed"); 
		exit(EXIT_FAILURE); 
	} 
	
	memset(&servaddr, 0, sizeof(servaddr)); 
	memset(&cliaddr, 0, sizeof(cliaddr)); 
	
	servaddr.sin_family = AF_INET; // IPv4 
	servaddr.sin_addr.s_addr = inet_addr(UPF_SERVER_IP);
	servaddr.sin_port = htons(PORT);
	
	// Bind the socket with the server address 
	if ( bind(sockfd, (const struct sockaddr *)&servaddr, 
			sizeof(servaddr)) < 0 ) 
	{ 
		perror("bind failed"); 
		exit(EXIT_FAILURE); 
	} 
	bool ran_address = false;
	while(1) {
		int len, n;
		len = sizeof(cliaddr);
		uint32_t decapLength;
		//cout << BOLDYELLOW <<"UPF: Reading from RAN .... " << RESET << endl; 
		n = recvfrom(sockfd, (char *)buffer, MAXLINE, 
			MSG_WAITALL, ( struct sockaddr *) &cliaddr, 
			(socklen_t*)&len); 
		buffer[n] = '\0'; 

		
		if(!ran_address) {
			memset(&ranaddr, 0, sizeof(ranaddr)); 
			ranaddr.sin_family = AF_INET;
			ranaddr.sin_port = cliaddr.sin_port;
			ranaddr.sin_addr.s_addr = inet_addr(inet_ntoa(cliaddr.sin_addr));
			std:: thread t(thread_func,tun1,sockfd);
			t.detach();

			ran_address = true;
		}
		
		//cout <<"UPF: Number of bytes read by UPF = "<< n << endl;
		char innerPacket[MAXLINE];
        if(decapsulateGtpMessage(buffer,n, innerPacket, 
				&decapLength) == FAILURE)
		{
			// cout<< "UPF: Error in decoding GTP message, cannot forward to"
			// 		"tun device" << endl;
			return 0;
		}
        memset(buffer,0,sizeof(buffer));

		// cout << BOLDYELLOW <<"UPF: Writing to tun interface again towards"
		// 		"DN network" << RESET << endl;
		tun1.sendData(innerPacket, decapLength);
		memset(buffer,0,sizeof(buffer));
	}    
	 
	return 0;
} 

int decapsulateGtpMessage(char * buffer,int len, char *innerPacket, 
		uint32_t *decapLength) 
{
    gtpMessage gtpMsg;
    if(decodeGtpMessage((uint8_t*)buffer,&gtpMsg,len) == FAILURE) 
    {
        //cout <<"UPF: decodeGtpMessage() failed "<<endl;
		return FAILURE;
    }
	//cout <<"UPF: decoded TEID = "<< gtpMsg.gtp_header.teid << endl;
	//cout<<"UPF: decoded seq num = "<< gtpMsg.gtp_header.seqNo<<endl;
	globalSeqNum = gtpMsg.gtp_header.seqNo;
	memcpy(innerPacket, gtpMsg.payload, gtpMsg.payloadLength);
	(*decapLength) = gtpMsg.payloadLength;
    return SUCCESS;
}

int encapsulateGtpMessage(char * innerPacket,int len, char *buffer, 
		uint32_t *encapLength)
{
	gtpMessage gtpMsg;
	/*copy the payload first */
	gtpMsg.payloadLength = len/* strlen(ipPayload) */;
	memcpy(&gtpMsg.payload,innerPacket,len/* strlen(ipPayload) */);
	/*fill the header */
	gtpMsg.gtp_header.flags = 0b00110010;
	gtpMsg.gtp_header.msgType = 0xFF;
	gtpMsg.gtp_header.length = len;
	if(gtpMsg.gtp_header.flags & (GTP_S_MASK|GTP_PN_MASK|GTP_E_MASK))
	{
		gtpMsg.gtp_header.length += GTP_HDR_OPTIONAL_FIELD_LENGTH;
	}
	globalSeqNum = globalSeqNum + (uint16_t)1;
	gtpMsg.gtp_header.seqNo = globalSeqNum;
	gtpMsg.gtp_header.teid = 202; // TODO: dynamically add destination TEID
		
	// TODO: check if above length is correct
	//cout << "UPF: Header length = "<<gtpMsg.gtp_header.length<<endl;
	
	uint32_t encodedLen = 0;
	memset(buffer,0,MAXLINE);
	if(encodeGtpMessage((uint8_t *)buffer, MAXLINE, &gtpMsg, 
			&encodedLen) == FAILURE)
	{
		//cout <<"UPF: encodeGtpMessage failed"<<endl;
		return FAILURE;
	}
	(*encapLength) = encodedLen;
	return SUCCESS;
}

