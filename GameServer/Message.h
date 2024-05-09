#pragma once
#include <string>

class ClientInfo;

class Message
{
public:
	Message();
	Message(const char* packet, ClientInfo* pClientInfo);
public:
	char opcode[8] = { 0 };
	char value[32] = { 0 };
	class ClientInfo* pClientInfo = nullptr;
};
