#include "UdpImpl.h"

UdpImpl::UdpImpl():m_socket(INVALID_SOCKET),
m_port(7289),
m_remotePort(7290),
m_strRemoteIp("127.0.0.1")
{

}


UdpImpl::~UdpImpl()
{
}

int UdpImpl::init(int listenPort, int remotePort, std::string remoteIp)
{
	
	m_port = listenPort;
	m_remotePort = remotePort;
	m_strRemoteIp = remoteIp;
	
	memset(&m_remoteAddr, 0, sizeof(sockaddr_in));
	m_remoteAddr.sin_family = AF_INET;
	m_remoteAddr.sin_addr.s_addr = inet_addr(m_strRemoteIp.c_str());
	m_remoteAddr.sin_port = htons(htons(m_remotePort));


	m_socket = socket(AF_INET, SOCK_DGRAM, 0); /* create a socket */
	if (m_socket == INVALID_SOCKET)
		return 0;

	if (bind() <= 0) return -1;
	
	if (setoption() <= 0) return -2;

	return 1;
}

int UdpImpl::finish()
{
	if (m_socket != INVALID_SOCKET)
		closesocket(m_socket);
	m_socket = INVALID_SOCKET;

	return 1;
}

int UdpImpl::bind()
{
	if (m_socket == INVALID_SOCKET) return 0;
	sockaddr_in addr_in;
	memset(&addr_in, 0, sizeof(sockaddr_in));
	addr_in.sin_family = AF_INET;
	addr_in.sin_addr.s_addr = htonl(INADDR_ANY);
	addr_in.sin_port = htons(htons(m_port));

	/* bind address and port to socket */
	if (::bind(m_socket, (struct sockaddr *)&addr_in, sizeof(addr_in)) == -1)
	{
		perror("bind error");
		return -1;
	}

	return 1;
}

 

int UdpImpl::setoption()
{
	// 设置端口重用
	int flag = 1;
	if (setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, (const char FAR*)&flag, sizeof(int) == -1))
	{
		perror("setsockopt");
		return -1;
	}

	// 设置Socket为非阻塞模式
		int iMode = 1;
	int retVal = ioctlsocket(m_socket, FIONBIO, (u_long FAR*) &iMode);
	if (retVal == SOCKET_ERROR)
	{
		printf("ioctlsocket failed!\n");
		return -2;
	}
	return 1;
}

int UdpImpl::sendData(char* buf, int bufsize)
{
	if (m_socket == INVALID_SOCKET) return -1;

	return sendto(m_socket, buf, bufsize,0, (sockaddr*)&m_remoteAddr,sizeof(sockaddr));
}

int UdpImpl::recvData(char* buf, int bufsize)
{
	sockaddr_in recvaddr;
	int len = sizeof(recvaddr);
	return recvfrom(m_socket, buf, bufsize, 0, (sockaddr*)&recvaddr, &len);
}
