//ServerTest.h
#pragma once		//��#ifdefЧ��һ��
#include <WinSock2.h>
#include <vector>
#include <thread>
#include <mutex>
#include<set>

#pragma comment(lib, "ws2_32.lib")  //���� ws2_32.dll

typedef struct serverThread
{
	std::thread* t1 = nullptr;
	bool isRuning = false;
	int threadID = -1;
	SOCKET csocket = -1;

	unsigned short top = 4;
	char Arr[71];
	serverThread() {
		memset(Arr, '0',70);
	}
}Sthread;

class SocketServerTest
{
public:
	SocketServerTest();
	~SocketServerTest();

	bool CreateSocket();//����socket
	bool BandSocket(const char* ip, const unsigned short prot);//�󶨱��ص�ַ�Ͷ˿ں�
	bool ListenSocket();//��ʼ������������������ӵȴ�����
	void AcceptSocketManager();//�����������ӵ����󣬲����ؾ��
	void AddClientSocket(SOCKET& sClient);//ѭ�����ͻ�������
	void ThreadClientRecv(Sthread* sthread);//�ͻ����߳̽�������
	void CloseMySocket();//�ر�����
	void OutputMessage(const char* outstr);//�������

private:
	SOCKET m_nServerSocket;//�󶨱��ص�ַ�Ͷ˿ںŵ��׽ӿ�
	std::vector<Sthread*> m_Vecthread;	//�����߳����ݵ�һ��vector
	int m_CsocketCount = 0;//�̱߳��
};
