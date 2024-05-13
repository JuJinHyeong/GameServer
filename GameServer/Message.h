#pragma once
#include <string>

class ClientInfo;

class Message
{
public:
	Message();
	Message(ClientInfo* pClientInfo, const char* packet, int packetSize);
public:
	std::string opcode;
	std::string data;
	class ClientInfo* pClientInfo = nullptr;
};
