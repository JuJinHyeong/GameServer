#pragma once
#include <string>

class ClientInfo;

class Message
{
public:
	Message();
	Message(const char* packet, ClientInfo* pClientInfo);
public:
	char opcode[4] = { 0 };
	char value[28] = { 0 };
	class ClientInfo* pClientInfo = nullptr;
};
