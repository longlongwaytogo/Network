// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� UDPSYNC_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// UDPSYNC_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#ifdef UDPSYNC_EXPORTS
#define UDPSYNC_API __declspec(dllexport)
#else
#define UDPSYNC_API __declspec(dllimport)
#endif

// �����Ǵ� udpSync.dll ������
class  UDPSYNC_API UdpSync {

protected:
	UdpSync();

public:
	static UdpSync& instance();
	~UdpSync();
	 bool initNetwork();
	 bool finishNetwork();

private:
	bool	m_bInit;
};
 