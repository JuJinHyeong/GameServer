#pragma once
#pragma comment(lib, "ws2_32.lib")
#include <WinSock2.h>
#include <WS2tcpip.h>

#include <vector>
#include <thread>

#include "ThreadSafeQueue.h"

constexpr UINT32 MAX_SOCKET_BUFFER_SIZE = 1024;
constexpr UINT32 MAX_WORKER_THREAD_COUNT = 4;
constexpr UINT32 MAX_DISPATCH_THREAD_COUNT = 4;
constexpr UINT16 SERVER_PORT = 9000;
constexpr UINT32 MAX_CLIENT = 100;

enum eIOOperation {
	RECV,
	SEND
};

struct OverlappedEx {
	WSAOVERLAPPED Overlapped;
	SOCKET ClientSocket;
	WSABUF WSABuf;
	char Buffer[MAX_SOCKET_BUFFER_SIZE];
	enum eIOOperation OperationType;
};

struct ClientInfo {
	SOCKET ClientSocket = INVALID_SOCKET;
	OverlappedEx RecvOverlapped = { 0 };
	OverlappedEx SendOverlapped = { 0 };
};

struct Message {
	std::string Message;
	ClientInfo* pClientInfo;
};


class GameServer {
public:
	GameServer();
	~GameServer();
	bool InitSocket();
	bool BindAndListen();
	bool StartServer();
	void DestroyThread();

private:
	void CreateClient();
	bool CreateWorkerThread();
	bool CreateAcceptThread();
	bool CreateDispatchThread();

	ClientInfo* GetEmptyClientInfo();
	bool BindIOCompletionPort(ClientInfo* pClientInfo);
	bool BindRecv(ClientInfo* pClientInfo);
	bool SendMsg(ClientInfo* pClientInfo, const char* message, const int messageLength);
	void CloseSocket(ClientInfo* pClientInfo, bool bIsForce = false);

private:
	void WorkerThread();
	void AcceptThread();
	void DispatchThread();

private:
	std::vector<ClientInfo> mClientInfoArr;
	ThreadSafeQueue<Message> mMessageQueue;
	std::thread mAcceptThread;
	std::vector<std::thread> mIOWorkerThreadArr;
	std::vector<std::thread> mDispatchThreadArr;

	HANDLE mIOCPHandle = INVALID_HANDLE_VALUE;
	SOCKET mListenSocket = INVALID_SOCKET;
	unsigned int mClientCount = 0;
	bool mbIsWorkerRun = true;
	bool mbIsAcceptRun = true;
	bool mbIsDispatchRun = true;
};