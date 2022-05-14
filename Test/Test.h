#pragma once

#include "resource.h"
//multipartiteClientSocket.h
#pragma once
#include <WinSock2.h>
#include <iostream>
#include <thread>

#pragma comment(lib, "ws2_32.lib")  //���� ws2_32.dll

typedef struct thread1
{
	std::thread* t1 = nullptr;
	bool isRuning = false;
} Mythread;

class MultipartiteClientSocketTest
{
public:
	MultipartiteClientSocketTest();
	~MultipartiteClientSocketTest();

	bool CreateSocket();
	void CloseSocket();

	bool Myconnect(const char* ip, const unsigned short prot);
	void Mysend();
	void Myrecv();
	bool InitMyrecv();

	void OutputMessage(const char* outstr);

private:
	char m_message[256];

	SOCKET m_nLocalSocket;
	Mythread recvThread;
};

MultipartiteClientSocketTest::MultipartiteClientSocketTest()
{
	m_nLocalSocket = -1;
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		OutputMessage("Socket�汾����ʧ��");
}

MultipartiteClientSocketTest::~MultipartiteClientSocketTest()
{
	CloseSocket();
}

void MultipartiteClientSocketTest::CloseSocket()
{
	if (m_nLocalSocket != -1)
		closesocket(m_nLocalSocket);	//�ر�socket����

	m_nLocalSocket = -1;
	WSACleanup();	//��ֹws2_32.lib��ʹ��
}

//����һ��socket
bool MultipartiteClientSocketTest::CreateSocket()
{
	if (m_nLocalSocket == -1)
	{
		m_nLocalSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (m_nLocalSocket != INVALID_SOCKET)
		{
			OutputMessage("�ͷ���socket�����ɹ�");
			return true;
		}
		else
		{
			OutputMessage("�ͷ���socket����ʧ��");
			return false;
		}
	}

	OutputMessage("�ͷ���socket������");
	return true;
}

bool MultipartiteClientSocketTest::Myconnect(const char* ip, const unsigned short prot)
{
	int nRet = SOCKET_ERROR;
	if (m_nLocalSocket != -1)
	{
		sockaddr_in m_nServeraddr;
		memset(&m_nServeraddr, 0, sizeof(m_nServeraddr));
		m_nServeraddr.sin_family = AF_INET;
		m_nServeraddr.sin_port = htons(prot);
		m_nServeraddr.sin_addr.s_addr = inet_addr(ip);
		nRet = connect(m_nLocalSocket, (sockaddr*)&m_nServeraddr, sizeof(m_nServeraddr));//�ɹ�����0�����򷵻�SOCKET_ERROR

		if (nRet == SOCKET_ERROR)
		{
			OutputMessage("����������ʧ�ܣ�");
			return false;
		}

		OutputMessage("���������ӳɹ���");
		InitMyrecv();

		return true;
	}

	return false;
}

bool MultipartiteClientSocketTest::InitMyrecv()
{
	if (m_nLocalSocket == -1)
		return false;

	if (recvThread.t1 == nullptr)
	{
		recvThread.isRuning = true;
		std::thread t(&MultipartiteClientSocketTest::Myrecv, this);
		t.detach();
		recvThread.t1 = &t;
	}
	else
	{
		OutputMessage("recvThread is failed��");
		return false;
	}

	Mysend();

	return true;
}

void MultipartiteClientSocketTest::Myrecv()
{
	if (m_nLocalSocket != -1)
	{
		int resultRecv = -1;
		while (recvThread.isRuning == true)
		{
			resultRecv = recv(m_nLocalSocket, m_message, sizeof(m_message), 0);
			if (resultRecv > 0)
			{
				//�����Ϣ
				OutputMessage(m_message);
				memset(m_message, '\0', sizeof(m_message));
			}
			else
			{
				//�⼸�ִ����룬��Ϊ�����������ģ���������
				if ((resultRecv < 0) && (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR))
				{
					continue;//������������
				}
				OutputMessage("������������жϣ�");
				break;//��������ѭ��
			}
		}
	}
	else
	{
		OutputMessage("��ǰ�������δ���ӣ�");
	}
	recvThread.t1 = nullptr;
	return;
}

void MultipartiteClientSocketTest::Mysend()
{
	if (m_nLocalSocket != -1)
	{
		char tempstr[3];
		int t;
		while (std::cin>>t,tempstr[0] = t)
		{
			std::cin >> t, tempstr[1] = t;
			send(m_nLocalSocket, tempstr, sizeof(tempstr), 0);
		}
	}
	else
	{
		OutputMessage("��ǰ�������δ����");
	}
	return;
}

void MultipartiteClientSocketTest::OutputMessage(const char* outstr)
{
	std::cout << outstr << std::endl;
}
