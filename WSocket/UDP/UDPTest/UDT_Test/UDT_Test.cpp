// UDT_Test.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"


#ifndef WIN32
#include <cstdlib>
#include <netdb.h>
#else
#include <winsock2.h>
#include <ws2tcpip.h>
#endif
#include <fstream>
#include <iostream>
#include <cstring>
#include <udt.h>1


using namespace std;


#ifndef WIN32
void* sendfile(void*);
#else
DWORD WINAPI sendfile(LPVOID);
#endif


int main(int argc, char* argv[])
{
	//usage: sendfile [server_port]
	if ((2 < argc) || ((2 == argc) && (0 == atoi(argv[1]))))
	{
		cout << "usage: sendfile [server_port]" << endl;
		return 0;
	}


	// use this function to initialize the UDT library
	// ��ʼ��UDT��
	UDT::startup();


	// ��ַ��Ϣ��ʾ
	addrinfo hints;


	// ʵ��ʹ�õĵ�ַ��Ϣָ��
	addrinfo* res;


	// �ڴ��ʼ����ȫ����Ϊ0
	memset(&hints, 0, sizeof(struct addrinfo));


	// ���ϱ���ʾ��ai_flagsdeֵ��ΧΪ0~7��ȡ���ڳ����������3����־λ��
	// ��������ai_flagsΪ ��AI_PASSIVE|AI_CANONNAME����ai_flagsֵ��Ϊ3�����������ĺ���ֱ�Ϊ��
	// (1)AI_PASSIVE���˱�־��λʱ����ʾ�����߽���bind()����������ʹ�÷��صĵ�ַ�ṹ�����˱�־����λʱ����ʾ����connect()����������ʹ�á�
	//    ���ڵ���λNULL���Ҵ˱�־��λ���򷵻صĵ�ַ����ͨ���ַ������ڵ���NULL���Ҵ˱�־����λ���򷵻صĵ�ַ���ǻػ���ַ��
	// (2)AI_CANNONAME���˱�־��λʱ���ں��������صĵ�һ��addrinfo�ṹ�е�ai_cannoname��Ա�У�
	//    Ӧ�ð���һ���Կ��ַ���β���ַ������ַ����������ǽڵ�������������
	// (3)AI_NUMERICHOST���˱�־��λʱ���˱�־��ʾ�����еĽڵ���������һ�����ֵ�ַ�ַ�����
	hints.ai_flags = AI_PASSIVE;


	// AF_UNIX������ͨ�ţ�
	// AF_INET��TCP/IP �C IPv4��
	// AF_INET6��TCP/IP �C IPv6��
	hints.ai_family = AF_INET;


	// SOCK_STREAM�� �ṩ�������ӵ��ȶ����ݴ��䣬��TCPЭ�顣
	// OOB�� ���������ݴ���ǰ����ʹ��connect()����������״̬��
	// SOCK_DGRAM�� ʹ�ò��������ɿ������ݰ����ӡ�
	// SOCK_SEQPACKET�� �ṩ�����ɿ������ݰ����ӡ�
	// SOCK_RAW�� �ṩԭʼ����Э���ȡ��
	// SOCK_RDM�� �ṩ�ɿ������ݰ����ӡ�
	// SOCK_PACKET�� ��������������ֱ��ͨ�š�
	//////////// ע���˴���SOCK_STREAM�����Ǳ�ʾUDT����ʹ��TCP���͵�Socket���ڵײ㽫��ת��ΪUDT_STREAM
	//////////// ������UDT�н�֧��SOCK_STREAM��SOCK_DGRAM���ֱ��ӦUDT_STREAM��UDT_DGRAM
	hints.ai_socktype = SOCK_STREAM;


	// Ĭ�Ϸ���˿�
	string service("9000");
	if (2 == argc)
		service = argv[1];


	// ���ݵ�ַ��Ϣ��ʾ����ʵ�ʿ��õĵ�ַ��Ϣ
	if (0 != getaddrinfo(NULL, service.c_str(), &hints, &res))
	{
		cout << "illegal port number or port is busy.\n" << endl;
		return 0;
	}


	// ����ʵ��ʹ�õĵ�ַ��Ϣ����һ��UDT��socket
	// �˴�ʵ�������յ�����CUDTUnited��newSocket����һ�������ᱻֱ�����õ�CUDT��m_iIPversion���ڶ��������ᱻӳ��ΪUDT���������ͣ����������������ԣ�û��ʵ������
	// Gu˵�˺���ԭ�;���������BSD��Socketһ�£����һЩ��ϵͳû����Ĳ���Ҳ���հ��˽�������
	UDTSOCKET serv = UDT::socket(res->ai_family, res->ai_socktype, res->ai_protocol);


	// Windows UDP issue
	// For better performance, modify HKLM\System\CurrentControlSet\Services\Afd\Parameters\FastSendDatagramThreshold
	// Windows�µ�UDP������
	// �����Ҫ���Ч�ʣ������޸�ע���HKLM\System\CurrentControlSet\Services\Afd\Parameters\FastSendDatagramThreshold
#ifdef WIN32
	int mss = 1052;


	// ��һ������ΪҪ���õ�UDTSocket
	// �ڶ�������0�ǻᱻ���Եģ�û��ʵ������
	// ����������ΪҪ���õĲ����������¼���ѡ�����enum UDTOpt��
	//     UDT_MSS ����䵥λ
	//     UDT_SNDSYN �Ƿ���������
	//     UDT_RCVSYN �Ƿ���������
	//     UDT_CC �Զ���ӵ�������㷨
	//     UDT_FC ���ڴ�С
	//     UDT_SNDBUF ���Ͷ��л������ֵ
	//     UDT_RCVBUF UDT���ջ����С
	//     UDT_LINGER �ر�ʱ�ȴ����ݷ������
	//     UDP_SNDBUF UDP���ͻ����С
	//     UDP_RCVBUF UDP���ջ����С
	//     UDT_RENDEZVOUS �������ģʽ
	//     UDT_SNDTIMEO send()��ʱ
	//     UDT_RCVTIMEO recv()��ʱ
	//     UDT_REUSEADDR ����һ���Ѵ��ڵĶ˿ڻ��ߴ���һ���µĶ˿�
	//     UDT_MAXBW ��ǰ���ӿ���ʹ�õ�������(bytes per second)
	// ���ĸ������ǲ���ֵ
	// ���������Ϊ����ֵ���ȣ��ڵײ�Ҳ�ᱻ���ԣ�û������
	UDT::setsockopt(serv, 0, UDT_MSS, &mss, sizeof(int));
#endif


	// �󶨶˿ڣ���һ������ΪSocket���ڶ���Ϊ�󶨵�ַ��������Ϊ��ַ���ȣ��ײ������ж��Ƿ����IPv4��IPv6��׼�ĳ��ȣ�
	if (UDT::ERROR == UDT::bind(serv, res->ai_addr, res->ai_addrlen))
	{
		cout << "bind: " << UDT::getlasterror().getErrorMessage() << endl;
		return 0;
	}


	// �ͷ�ͨ��getaddrinfo����ĵ�ַ��Ϣ
	freeaddrinfo(res);


	cout << "server is ready at port: " << service << endl;


	// ��ʼ��������һ������ΪSocket���ڶ�������Ϊ��������������������
	UDT::listen(serv, 10);


	// �ͻ��˵�ַ��Ϣ
	sockaddr_storage clientaddr;
	// ��ַ��Ϣ���ݳ���
	int addrlen = sizeof(clientaddr);


	// �������ӵ�Socket
	UDTSOCKET fhandle;


	while (true)
	{
		// �����������󣬵�һ������ΪSocket���ڶ�������Ϊ���󷽵�ַ��Ϣ������������Ϊ��ַ��Ϣ���ȣ�
		// �ڶ����͵����������������õ�ַ�ռ䣬���ܷ�����Ϣ
		if (UDT::INVALID_SOCK == (fhandle = UDT::accept(serv, (sockaddr*)&clientaddr, &addrlen)))
		{
			cout << "accept: " << UDT::getlasterror().getErrorMessage() << endl;
			return 0;
		}


		// ͨ��getnameinfo()����ַ��Ϣת��Ϊ�ڵ��ַ���˿���Ϣ
		char clienthost[NI_MAXHOST];
		char clientservice[NI_MAXSERV];
		getnameinfo((sockaddr *)&clientaddr, addrlen, clienthost, sizeof(clienthost), clientservice, sizeof(clientservice), NI_NUMERICHOST | NI_NUMERICSERV);
		cout << "new connection: " << clienthost << ":" << clientservice << endl;


		// ���ӳɹ�������sendfile�̣߳����벶�񵽵�Socket������ʹ���˿������촴��һ���µĶ���ʹ��fhandle�����ʹ�ò����ܵ��̵߳�Ӱ��
#ifndef WIN32
		pthread_t filethread;
		pthread_create(&filethread, NULL, sendfile, new UDTSOCKET(fhandle));
		pthread_detach(filethread);
#else
		CreateThread(NULL, 0, sendfile, new UDTSOCKET(fhandle), 0, NULL);
#endif
	}


	// �رռ���Socket
	UDT::close(serv);


	// use this function to release the UDT library
	// �ͷ�UDT��
	UDT::cleanup();


	return 0;
}


#ifndef WIN32
void* sendfile(void* usocket)
#else
DWORD WINAPI sendfile(LPVOID usocket)
#endif
{
	// ��ȡSocket�������ԭ�����Socket
	UDTSOCKET fhandle = *(UDTSOCKET*)usocket;
	delete (UDTSOCKET*)usocket;


	// aquiring file name information from client
	// ������˻���ļ���
	char file[1024];
	int len;


	// ����һ�����ݰ�������Ϊһ��intֵ������Ϊint�ĳ��ȣ���ֵ������len��
	// ��һ������ΪSocket���ڶ�������Ϊ���ݱ����ַ������������Ϊ���ݳ��ȣ����ĸ������ײ����ԣ�û������
	if (UDT::ERROR == UDT::recv(fhandle, (char*)&len, sizeof(int), 0))
	{
		cout << "recv: " << UDT::getlasterror().getErrorMessage() << endl;
		return 0;
	}


	// ��ʼ������Ϣ��ͨ�������õĳ������ж���Ϣ�Ĵ�С
	if (UDT::ERROR == UDT::recv(fhandle, file, len, 0))
	{
		cout << "recv: " << UDT::getlasterror().getErrorMessage() << endl;
		return 0;
	}


	// �����յ����ַ����ӽ�����
	file[len] = '\0';


	// open the file
	// ������ָ�����ļ����Զ����Ʒ�ʽ��ȡ��
	fstream ifs(file, ios::in | ios::binary);


	// ���ļ�ָ�붨λ���ļ�ĩβ
	ifs.seekg(0, ios::end);
	// ����ļ���С
	int64_t size = ifs.tellg();
	// ���ļ�ָ�붨λ���ļ���ͷ
	ifs.seekg(0, ios::beg);


	// send file size information
	// �����󷽷����ļ�����
	// ��һ������ΪSocket���ڶ�������Ϊ���ݵ�ַ������������Ϊ���ݳ��ȣ����ĸ������ᱻ�ײ���ԣ�û������
	if (UDT::ERROR == UDT::send(fhandle, (char*)&size, sizeof(int64_t), 0))
	{
		cout << "send: " << UDT::getlasterror().getErrorMessage() << endl;
		return 0;
	}


	// ��ʼ���������ݼ�CPerfMon
	UDT::TRACEINFO trace;
	UDT::perfmon(fhandle, &trace);


	// send the file
	// ƫ����Ϊ0����ʼ�����ļ�
	// ��һ������ΪSocket���ڶ�������Ϊ�ļ���������������Ϊƫ���������ĸ�����Ϊ�ļ���С
	int64_t offset = 0;
	if (UDT::ERROR == UDT::sendfile(fhandle, ifs, offset, size))
	{
		cout << "sendfile: " << UDT::getlasterror().getErrorMessage() << endl;
		return 0;
	}


	// ��ȡ������Ϣ�����ݼ�CPerfMon
	// �ó�����Ч��
	UDT::perfmon(fhandle, &trace);
	cout << "speed = " << trace.mbpsSendRate << "Mbits/sec" << endl;


	// �ر�����Socket
	UDT::close(fhandle);


	// �ر��ļ���
	ifs.close();


#ifndef WIN32
	return NULL;
#else
	return 0;
#endif
}
int main()
{
    return 0;
}

