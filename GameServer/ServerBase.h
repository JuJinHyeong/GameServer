#pragma once
#pragma comment(lib, "ws2_32.lib")
#include <WinSock2.h>
#include <WS2tcpip.h>

#include <vector>
#include <thread>

#include "ThreadSafeQueue.h"
#include "Message.h"
#include "ClientInfo.h"

class ServerBase {
public:
	ServerBase();
	virtual ~ServerBase();
	bool Initialize();
	bool InitSocket();
	bool BindAndListen();
	bool StartServer();
	void DestroyThread();

protected:
	virtual void OnConnect(ClientInfo* pClientInfo) = 0;
	virtual void OnReceive(ClientInfo* pClientInfo, const char* message, const int messageLength) = 0;
	virtual void OnClose(ClientInfo* pClientInfo) = 0;

private:
	void CreateClient();
	bool CreateWorkerThread();
	bool CreateAcceptThread();
	bool CreateDispatchThread();

	ClientInfo* GetEmptyClientInfo();
	bool BindIOCompletionPort(ClientInfo* pClientInfo);
	void CloseSocket(ClientInfo* pClientInfo, bool bIsForce = false);

protected:
	void AcceptThread();
	void WorkerThread();
	virtual void DispatchThread();
	bool BindRecv(ClientInfo* pClientInfo);
	bool BindSend(ClientInfo* pClientInfo, const char* message, const int messageLength);

protected:
	ThreadSafeQueue<Message> mMessageQueue;
	bool mbIsWorkerRun = true;
	bool mbIsAcceptRun = true;
	bool mbIsDispatchRun = true;

private:
	std::vector<ClientInfo> mClientInfoArr;
	unsigned int mClientCount = 0;

	std::thread mAcceptThread;
	std::vector<std::thread> mIOWorkerThreadArr;
	std::vector<std::thread> mDispatchThreadArr;

	HANDLE mIOCPHandle = INVALID_HANDLE_VALUE;
	SOCKET mListenSocket = INVALID_SOCKET;

private:
	constexpr static UINT32 MAX_WORKER_THREAD_COUNT = 4;
	constexpr static UINT32 MAX_DISPATCH_THREAD_COUNT = 4;
	constexpr static UINT16 SERVER_PORT = 9000;
	constexpr static UINT32 MAX_CLIENT = 100;
};