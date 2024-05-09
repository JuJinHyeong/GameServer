#include "GameServer.h"
#include <iostream>
#include "ClientInfo.h"

GameServer::GameServer()
	:
	mIOCP(mClientInfoArr, mMessageQueue),
	mDispatcher(mIOCP, mMessageQueue)
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
	mDispatcher.CreateThread();

	std::cout << "Start Server Success\n";
	return true;
}

void GameServer::DestroyThread() {
	mIOCP.DestoryThread();
	mDispatcher.DestoryThread();
}

void GameServer::CreateClient() {
	for (UINT32 i = 0; i < MAX_CLIENT; ++i) {
		mClientInfoArr.emplace_back();
	}
}
