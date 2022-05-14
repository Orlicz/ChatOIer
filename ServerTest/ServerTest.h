//ServerTest.h
#pragma once		//和#ifdef效果一样
#include <WinSock2.h>
#include <vector>
#include <thread>
#include <mutex>
#include<set>

#pragma comment(lib, "ws2_32.lib")  //加载 ws2_32.dll

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

	bool CreateSocket();//创建socket
	bool BandSocket(const char* ip, const unsigned short prot);//绑定本地地址和端口号
	bool ListenSocket();//初始化监听并设置最大连接等待数量
	void AcceptSocketManager();//接受请求连接的请求，并返回句柄
	void AddClientSocket(SOCKET& sClient);//循环检查客户端连接
	void ThreadClientRecv(Sthread* sthread);//客户端线程接受数据
	void CloseMySocket();//关闭连接
	void OutputMessage(const char* outstr);//输出文字

private:
	SOCKET m_nServerSocket;//绑定本地地址和端口号的套接口
	std::vector<Sthread*> m_Vecthread;	//保存线程数据的一个vector
	int m_CsocketCount = 0;//线程编号
};
