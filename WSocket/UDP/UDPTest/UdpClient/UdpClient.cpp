// UdpClient.cpp : 定义控制台应用程序的入口点。
//
// 客户端程序
#include "stdafx.h"

#include "../udpSync/udpSync.h"
#include "../udpSync/UdpImpl.h"
#include <iostream>
#include <string>
int main()
{
	UdpImpl udp;

	UdpSync::instance().initNetwork();
	int port = 5034;
	int remotePort = 5035;
	std::string dst = "127.0.0.1";
	if (udp.init(port, remotePort, dst) <= 0) return 0;
	
	std::cout << "send to server:" << std::endl;
	
	std::string info;
	char recvBuf[1024];
	while (1)
	{
		std::cin >> info;
		int ret = udp.sendData((char*)info.c_str(), info.size());
		std::cout << "send size: " << ret << "info:" << info << std::endl;


		memset(recvBuf, 0, 1024);
		ret = udp.recvData(recvBuf, 1024);
		std::cout << "recv size: " << ret << "info:" << recvBuf << std::endl;

		//Sleep(100);
		
	}

	UdpSync::instance().finishNetwork();
    return 0;
}

