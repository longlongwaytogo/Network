#pragma once

#include <string>
#include <winsock.h>

#ifdef UDPSYNC_EXPORTS
#define UDPSYNC_API __declspec(dllexport)
#else
#define UDPSYNC_API __declspec(dllimport)
#endif

class UDPSYNC_API UdpImpl
{
public:
	UdpImpl();
	~UdpImpl();
public:
	/*
		listenPort：本地监听端口，用于接收数据
		remotePort：远端监听端口，用于接收本机发送的数据
		reomteIp:	远端目标主机地址，用于发送数据使用
	*/
	int init(int listenPort,int remotePort,std::string remoteIp);
	int finish();
	
	int sendData(char* buf, int bufsize);
	int recvData(char* buf, int bufsize);

protected:
	int bind();
	int setoption();

private:
	int m_socket;
	unsigned short m_port;
	unsigned short m_remotePort;
	std::string	   m_strRemoteIp;
	sockaddr_in	   m_remoteAddr;
};

