// Udp_Connect.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <stdio.h>


#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <string>
#include <sstream>
using namespace std;

#pragma comment(lib,"ws2_32.lib")

#define socklen_t int

#define MAXLINE 80
#define SERV_PORT 8888




class Server
{

public:
	void do_echo(int sockfd, struct sockaddr *pcliaddr, socklen_t clilen)
	{
		int n;
		socklen_t len;
		char mesg[MAXLINE];

		for (;;)
		{
			memset(mesg, 0,MAXLINE);
			len = clilen;
			/* waiting for receive data */
			n = recvfrom(sockfd, mesg, MAXLINE, 0, pcliaddr, &len);
			/* sent data back to client */
			std::string info(mesg);
			cout << "recv:" << info << endl;
			sendto(sockfd, mesg, n, 0, pcliaddr, len);
		}
	}

	int main(void)
	{
		int sockfd;
		
		sockaddr_in servaddr, cliaddr;

		sockfd = socket(AF_INET, SOCK_DGRAM, 0); /* create a socket */

												 /* init servaddr */
		memset(&servaddr, 0, sizeof(servaddr));
		servaddr.sin_family = AF_INET;
		servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
		servaddr.sin_port = htons(SERV_PORT);

		/* bind address and port to socket */
		if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1)
		{
			perror("bind error");
			exit(1);
		}

		do_echo(sockfd, (struct sockaddr *)&cliaddr, sizeof(cliaddr));

		return 0;
	}

};

class Client
{
public:
void do_cli(FILE *fp, int sockfd, struct sockaddr *pservaddr, socklen_t servlen)
{
	int n;
	char sendline[MAXLINE], recvline[MAXLINE + 1];

	/* connect to server */
	if (connect(sockfd, (struct sockaddr *)pservaddr, servlen) == -1)
	{
		perror("connect error");
		exit(1);
	}

	while (fgets(sendline, MAXLINE, fp) != NULL)
	{
		/* read a line and send to server */
		//write(sockfd, sendline, strlen(sendline));
		send(sockfd, sendline, strlen(sendline), 0);
		/* receive data from server */
		//n = read(sockfd, recvline, MAXLINE);
		n = recv(sockfd, recvline, MAXLINE, 0);
		if (n == -1)
		{
			perror("read error");
			exit(1);
		}
		recvline[n] = 0; /* terminate string */
		fputs(recvline, stdout);
	}
}

int main(std::string ip)
{
	int sockfd;
	struct sockaddr_in servaddr;

	/* check args */
	if (ip.size() <= 0)
	{
		printf("usage: udpclient <IPaddress>\n");
		//exit(1);
		ip = "127.0.0.1";
	}

	/* init servaddr */
	memset(&servaddr,0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);
	if (inet_pton(AF_INET, ip.c_str(), &servaddr.sin_addr) <= 0)
	{
		printf("[%s] is not a valid IPaddress\n", ip.c_str());
		exit(1);
	}

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);

	do_cli(stdin, sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

	return 0;
 }
};

int main()
{
	WORD versionRequest = MAKEWORD(2, 2);
	WSAData wsaData;
	int err;
	//wsa startup  
	err = WSAStartup(versionRequest, &wsaData);
	
	cout << "select the role is server or client:" << endl;
	cout << "1: server,2:client" << endl;
	int role = 0;
	cin >> role;
	if (role == 1)
	{
		Server s;
		s.main();
	}
	else if (role ==  2)
	{
		Client c;
		std::string connectHost = "127.0.0.1";
		c.main(connectHost);

	}
	else
	{
		cout << " error select" << endl;
		WSACleanup();

		exit(0);
	}

	WSACleanup();
    return 0;
}

