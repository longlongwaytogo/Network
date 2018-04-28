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
		listenPort�����ؼ����˿ڣ����ڽ�������
		remotePort��Զ�˼����˿ڣ����ڽ��ձ������͵�����
		reomteIp:	Զ��Ŀ��������ַ�����ڷ�������ʹ��
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

