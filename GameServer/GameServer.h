#pragma once
#pragma comment(lib, "ws2_32.lib")
#include <WinSock2.h>
#include <WS2tcpip.h>

#include <vector>
#include <thread>

#include "ThreadSafeQueue.h"
#include "IOCompletionPort.h"

class ClientInfo;

constexpr UINT32 MAX_DISPATCH_THREAD_COUNT = 4;
constexpr UINT16 SERVER_PORT = 9000;
constexpr UINT32 MAX_CLIENT = 100;

class GameServer {
public:
	GameServer();
	~GameServer();
	bool Initialize();
	bool StartServer();
	void DestroyThread();

private:
	void CreateClient();
	bool CreateDispatchThread();

	bool BindRecv(ClientInfo* pClientInfo);
	bool SendMsg(ClientInfo* pClientInfo, const char* message, const int messageLength);

private:
	void DispatchThread();

private:
	std::vector<ClientInfo> mClientInfoArr;
	ThreadSafeQueue<Message> mMessageQueue;
	std::vector<std::thread> mDispatchThreadArr;

	bool mbIsDispatchRun = true;

	IOCompletionPort mIOCP;
};