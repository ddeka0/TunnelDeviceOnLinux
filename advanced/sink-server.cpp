/*
sudo ip route add 172.112.100.0/24 via 10.129.131.213 dev eno1
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

using namespace std;

#define PORT	 	8080
#define MAXLINE 	1024
#define DN_SERVER_IP "10.129.131.194"
// Driver code
int main() {
	int sockfd;
	char buffer[MAXLINE];
	struct sockaddr_in servaddr, cliaddr;
	// Creating socket file descriptor
	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}
	memset(&servaddr, 0, sizeof(servaddr));
	memset(&cliaddr, 0, sizeof(cliaddr));
	// Filling server information
	servaddr.sin_family = AF_INET; // IPv4
	servaddr.sin_addr.s_addr = inet_addr(DN_SERVER_IP);
	servaddr.sin_port = htons(PORT);

	// Bind the socket with the server address
	if ( bind(sockfd, (const struct sockaddr *)&servaddr,
			sizeof(servaddr)) < 0 )
	{
		perror("bind failed"); 
		exit(EXIT_FAILURE); 
	}

	int len,n;
	long long int cnt = 0;
	string hello = " Hello from server";
	string msg = to_string(cnt) + hello;


	while(1) {
		int len, n; 
		printf("reading ...\n");
		n = recvfrom(sockfd, (char *)buffer, MAXLINE, 
				MSG_WAITALL, ( struct sockaddr *) &cliaddr, 
				(socklen_t*)&len);
		buffer[n] = '\0'; 
		printf("Client : %s\n", buffer);
		printf("Client IP address is : %s\n",inet_ntoa(cliaddr.sin_addr));
		msg = msg +" + "+ std::string(buffer);
		sendto(sockfd,(msg.c_str()),msg.length(),
			MSG_CONFIRM,(const struct sockaddr *)(&cliaddr),
			sizeof(cliaddr));
		cnt++;
		string prefix = to_string(cnt);
		msg = prefix + hello;
		memset(buffer,0,sizeof(buffer));
	}
	return 0; 
} 

