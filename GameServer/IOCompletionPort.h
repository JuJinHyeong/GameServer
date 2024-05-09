#pragma once
#pragma comment(lib, "ws2_32.lib")
#include <Winsock2.h>
#include <WS2tcpip.h>
#include <thread>
#include <vector>
#include "ThreadSafeQueue.h"
#include "Message.h"

class ClientInfo;

constexpr UINT32 MAX_WORKER_THREAD_COUNT = 4;

class IOCompletionPort
{
public:
	IOCompletionPort(std::vector<ClientInfo>& clientInfoArr, ThreadSafeQueue<Message>& messageQueue);
	~IOCompletionPort();

	bool InitSocket();
	bool BindAndListen(const UINT16 serverPort);
	bool Create();
	bool BindWithClient(ClientInfo* pClientInfo);

	bool CreateWorkerThread();
	bool CreateAcceptThread();

	void DestoryThread();

	bool BindRecv(ClientInfo* pClientInfo);
	bool BindSend(ClientInfo* pClientInfo, const char* message, const int messageLength);

private:
	ClientInfo* GetEmptyClientInfo();
	void AcceptThread();
	void WorkerThread();

private:
	std::vector<std::thread> mIOWorkerThreadArr;
	// TODO: change to vector
	std::thread mAcceptThread;
	bool mbIsWorkerRun = true;
	bool mbIsAcceptRun = true;

	SOCKET mListenSocket = INVALID_SOCKET;
	HANDLE mIOCPHandle = nullptr;
	
private:
	ThreadSafeQueue<Message>& mMessageQueue;
	std::vector<ClientInfo>& mClientInfoArr;

};

