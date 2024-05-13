#include "EchoServer.h"
#include <iostream>

void EchoServer::OnConnect(ClientInfo* pClientInfo)
{
	std::cout << "Connected Client IP: " << pClientInfo->IP << std::endl;
}

void EchoServer::OnReceive(ClientInfo* pClientInfo, const char* buffer, int length)
{
	std::cout << "Received Packet: " << buffer << '\n';
	mMessageQueue.Push({ pClientInfo, buffer, length });
}

void EchoServer::OnClose(ClientInfo* pClientInfo)
{
	std::cout << "Disconnected Client IP: " << pClientInfo->IP << std::endl;
}
