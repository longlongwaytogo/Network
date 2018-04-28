// udpSync.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "UdpSync.h"
#include <WinSock2.h>

#pragma  comment(lib,"ws2_32.lib")

UdpSync::UdpSync():m_bInit(false)
{

}


UdpSync& UdpSync::instance()
{
	static UdpSync s_sync;
	return s_sync;
	
}

UdpSync::~UdpSync()
{

}



bool UdpSync::initNetwork()
{
	WORD versionRequest = MAKEWORD(2, 2);
	WSAData wsaData;
	int err;
	//wsa startup  
	err = WSAStartup(versionRequest, &wsaData);
	if (err == 0)
		m_bInit = true;
	return m_bInit;
}

bool UdpSync::finishNetwork()
{
	if(m_bInit)
		WSACleanup();
	return 1;
}
