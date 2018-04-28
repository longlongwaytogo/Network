#pragma once

#ifdef UDPSYNC_EXPORTS
#define UDPSYNC_API __declspec(dllexport)
#else
#define UDPSYNC_API __declspec(dllimport)
#endif

class UDPSYNC_API UdpClient
{
public:
	UdpClient();
	~UdpClient();
};

