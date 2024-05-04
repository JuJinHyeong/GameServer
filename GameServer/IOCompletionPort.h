#pragma once
#pragma comment(lib, "ws2_32.lib")
#include <WinSock2.h>
#include <WS2tcpip.h>

#include <vector>
#include <thread>

constexpr UINT32 MAX_SOCKET_BUFFER_SIZE = 1024;
constexpr UINT32 MAX_WORKER_THREAD_COUNT = 4;

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

class IOCompletionPort {
public:
	IOCompletionPort();
	~IOCompletionPort();
	bool InitSocket();
	bool BindAndListen(UINT16 bindPort);
	bool StartServer(const UINT32 maxClientCount);
	void DestroyThread();

private:
	void CreateClient(const UINT32 maxClientCount);
	bool CreateWorkerThread();
	bool CreateAcceptThread();
	ClientInfo* GetEmptyClientInfo();
	bool BindIOCompletionPort(ClientInfo* pClientInfo);
	bool BindRecv(ClientInfo* pClientInfo);
	bool SendMsg(ClientInfo* pClientInfo, const char* message, const int messageLength);
	void WorkerThread();
	void AcceptThread();
	void CloseSocket(ClientInfo* pClientInfo, bool bIsForce = false);

private:
	std::vector<ClientInfo> mClientInfoArr;
	SOCKET mListenSocket = INVALID_SOCKET;
	unsigned int mClientCount = 0;
	std::vector<std::thread> mIOWorkerThreadArr;
	std::thread mAcceptThread;
	HANDLE mIOCPHandle = INVALID_HANDLE_VALUE;
	bool mbIsWorkerRun = true;
	bool mbIsAcceptRun = true;
	char mSocketBuffer[1024] = { 0 };
};