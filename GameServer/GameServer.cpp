#include "GameServer.h"
#include <iostream>
#include "ClientInfo.h"

GameServer::GameServer()
	:
	mIOCP(mClientInfoArr, mMessageQueue)
{
}

GameServer::~GameServer() {
	WSACleanup();
}

bool GameServer::Initialize()
{
	mIOCP.InitSocket();
	mIOCP.BindAndListen(SERVER_PORT);
	mIOCP.Create();
	return true;
}

bool GameServer::StartServer() {
	CreateClient();
	mIOCP.Create();
	mIOCP.CreateAcceptThread();
	mIOCP.CreateWorkerThread();

	bool result = CreateDispatchThread();
	if (!result) {
		return result;
	}

	std::cout << "Start Server Success\n";
	return true;
}

void GameServer::DestroyThread() {
	mIOCP.DestoryThread();

	mbIsDispatchRun = false;
	for (std::thread& th : mDispatchThreadArr) {
		if (th.joinable()) {
			th.join();
		}
	}
}

void GameServer::CreateClient() {
	for (UINT32 i = 0; i < MAX_CLIENT; ++i) {
		mClientInfoArr.emplace_back();
	}
}

bool GameServer::CreateDispatchThread()
{
	for (UINT32 i = 0; i < MAX_DISPATCH_THREAD_COUNT; ++i) {
		mDispatchThreadArr.emplace_back([this]() { DispatchThread(); });
	}
	std::cout << "Create Dispatch Thread Success\n";
	return true;
}

bool GameServer::BindRecv(ClientInfo* pClientInfo) {
	DWORD flags = 0;
	DWORD recvBytes = 0;

	pClientInfo->RecvOverlapped.WSABuf.len = MAX_SOCKET_BUFFER_SIZE;
	pClientInfo->RecvOverlapped.WSABuf.buf = pClientInfo->RecvOverlapped.Buffer;
	pClientInfo->RecvOverlapped.OperationType = eIOOperation::RECV;

	int result = WSARecv(
		pClientInfo->ClientSocket,
		&(pClientInfo->RecvOverlapped.WSABuf),
		1,
		&recvBytes,
		&flags,
		(LPWSAOVERLAPPED) & (pClientInfo->RecvOverlapped),
		NULL
	);

	if (result == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
		std::cerr << "WSARecv failed with error: " << WSAGetLastError() << std::endl;
		return false;
	}
	return true;
}

bool GameServer::SendMsg(ClientInfo* pClientInfo, const char* message, const int messageLength) {
	DWORD recvNumBytes = 0;
	CopyMemory(pClientInfo->SendOverlapped.Buffer, message, messageLength + 1);

	pClientInfo->SendOverlapped.WSABuf.len = messageLength;
	pClientInfo->SendOverlapped.WSABuf.buf = pClientInfo->SendOverlapped.Buffer;
	pClientInfo->SendOverlapped.OperationType = eIOOperation::SEND;

	int result = WSASend(
		pClientInfo->ClientSocket,
		&(pClientInfo->SendOverlapped.WSABuf),
		1,
		&recvNumBytes,
		0,
		(LPWSAOVERLAPPED) & (pClientInfo->SendOverlapped),
		NULL
	);

	if (result == SOCKET_ERROR && WSAGetLastError() != ERROR_IO_PENDING) {
		std::cerr << "WSASend failed with error: " << WSAGetLastError() << std::endl;
		return false;
	}
	return true;
}

void GameServer::DispatchThread()
{
	while (mbIsDispatchRun) {
		Message message = mMessageQueue.Pop();

		std::cout << "Recv Message: " << message.Message << std::endl;

		SendMsg(message.pClientInfo, message.Message.c_str(), (int)message.Message.size());
	}
}
