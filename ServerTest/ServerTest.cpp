#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "ServerTest.h"
#include <iostream>
#include <string>





SocketServerTest::SocketServerTest() :m_nServerSocket(-1)
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		OutputMessage("Socket版本错误");
}

SocketServerTest::~SocketServerTest()
{
	CloseMySocket();
}

void SocketServerTest::CloseMySocket()
{
	//退出所有线程
	for (auto it : m_Vecthread)
	{
		it->isRuning = false;
	}

	if (m_nServerSocket != -1)
		closesocket(m_nServerSocket);	//关闭socket连接

	m_nServerSocket = -1;
	WSACleanup();	//终止ws2_32.lib的使用
}

bool SocketServerTest::CreateSocket()
{
	if (m_nServerSocket == -1)
	{
		m_nServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);	//设定TCP协议接口;失败返回INVALID_SOCKET
		if (m_nServerSocket != INVALID_SOCKET)
		{
			OutputMessage("服务器启动成功");
			return true;
		}
	}
	return false;
}

bool SocketServerTest::BandSocket(const char* ip, const unsigned short prot)
{
	int nRet = -1;
	if (m_nServerSocket != -1)
	{
		sockaddr_in Serveraddr;
		memset(&Serveraddr, 0, sizeof(sockaddr_in*));
		Serveraddr.sin_family = AF_INET;
		Serveraddr.sin_addr.s_addr = inet_addr(ip);
		Serveraddr.sin_port = htons(prot);
		nRet = bind(m_nServerSocket, (sockaddr*)&Serveraddr, sizeof(Serveraddr));	//绑定服务器地址和端口号;成功，返回0，否则为SOCKET_ERROR
	}

	if (nRet == 0)
	{
		OutputMessage("绑定IP和端口成功");
		return true;
	}

	OutputMessage("绑定IP和端口失败");
	return false;
}

bool SocketServerTest::ListenSocket()
{
	int nRet = -1;
	if (m_nServerSocket != -1)
	{
		nRet = listen(m_nServerSocket, 5);//设定接受连接的套接字，以及设定连接队列长度;成功返回0，失败返回-1
	}
	if (nRet == SOCKET_ERROR)
	{
		OutputMessage("监听绑定失败");
		return false;
	}

	OutputMessage("监听绑定成功");
	return true;
}

void SocketServerTest::AcceptSocketManager()
{
	while (m_nServerSocket != -1)
	{
		sockaddr_in nClientSocket;//如果要保存客户端的IP和端口号，就存在本地
		int nSizeClient = sizeof(nClientSocket);
		SOCKET sClient = accept(m_nServerSocket, (sockaddr*)&nClientSocket, &nSizeClient);//接受客户端连接，阻塞状态;失败返回-1
		if (sClient == SOCKET_ERROR)
		{
			OutputMessage("当前与客户端连接失败");
			return;
		}
		else
		{
			AddClientSocket(sClient);
		}
		Sleep(25);
	}
}

void SocketServerTest::AddClientSocket(SOCKET& sClient)
{
	Sthread* it = new Sthread();
	it->threadID = ++m_CsocketCount;
	it->isRuning = true;
	it->csocket = sClient;
	std::thread t(&SocketServerTest::ThreadClientRecv, this, it);
	t.detach();
	it->t1 = &t;
	m_Vecthread.push_back(it);
	char str[50];
	sprintf_s(str, "%dthread connect is success", it->threadID);
	OutputMessage(str);

	char mess[] = "sercer:与服务器连接成功！";
	send(sClient, mess, sizeof(mess), 0);//发送消息给客户端
}
void SocketServerTest::ThreadClientRecv(Sthread* sthread)
{
	while (sthread->isRuning == true)
	{
		// 从客户端接收数据
		char buff[3];
		int nRecv = recv(sthread->csocket, buff, 3, 0);//从客户端接受消息
		if (nRecv > 0)
		{
			char str[50];
			sprintf_s(str, "%dthread send message", sthread->threadID);
			OutputMessage(str);
			OutputMessage(buff);
#if 0
			if (buff[0] == 'A') {
				sthread->top++;
				char mess[] = "server:收到了你的消息。";
				send(sthread->csocket, mess, sizeof(mess), 0);
			}
			else {
				auto ad = std::stoi(std::string(buff+1));
				for (int i = 0; i < sthread->top; ++i) {
					if(buff[0] == 'X')
						sthread->xArr[i] += ad;
					else
						sthread->yArr[i] += ad;

				}
				std::string Mess = "";
				for (auto& thres : m_Vecthread) {
					for (auto i = 0; i < thres->top;++i) {
						Mess += '(';
						Mess += std::to_string(thres->xArr[i]);
						Mess += ',';
						Mess += std::to_string(thres->yArr[i]);
						Mess += ") ";
					}
					Mess += '\n';
				}
				send(sthread->csocket, Mess.c_str(), Mess.length(), 0);

			}
#endif
			
			if (buff[0] == 65) {
				char Sends[1024];
				memset(Sends, 0, 1024);
				int top = 0;
				for (auto& thres : m_Vecthread) {
					for (int i = 0; i < 64; ++i) {
						Sends[top++] = thres->Arr[i];
					}
					Sends[top++] = '\n';
				}
				send(sthread->csocket, Sends, top-1, 0);
			}
			else {
				sthread->Arr[buff[0]] = buff[1];
				char mess[] = "server:收到了你的消息。";
				send(sthread->csocket, mess, sizeof(mess), 0);
			}
			
		}
		else
		{
			char str[50];
			sprintf_s(str, "ID%d is exit", sthread->threadID);
			OutputMessage(str);
			sthread->isRuning = false;
		}
	}
	return;
}

void SocketServerTest::OutputMessage(const char* outstr)
{
	std::cout << outstr << std::endl;
}
int main(){
	SocketServerTest* sserverTest = new SocketServerTest();
	sserverTest->CreateSocket();
	sserverTest->BandSocket("127.0.0.1", 1234);
	sserverTest->ListenSocket();
	sserverTest->AcceptSocketManager();
	delete sserverTest;

	system("pause");
	return 0;
}