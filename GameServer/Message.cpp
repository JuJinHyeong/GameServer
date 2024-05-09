#include "Message.h"
#include "ClientInfo.h"

Message::Message()
{
}

Message::Message(const char* packet, ClientInfo* pClientInfo)
	:
	pClientInfo(pClientInfo)
{
	memcpy(opcode, packet, 3);
	memcpy(value, packet + 3, 27);
}
