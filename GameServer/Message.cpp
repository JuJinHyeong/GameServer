#include "Message.h"
#include "ClientInfo.h"

Message::Message()
	:
	opcode(),
	data(),
	pClientInfo(nullptr)
{
}

Message::Message(ClientInfo* pClientInfo, const char* packet, int packetSize)
	:
	pClientInfo(pClientInfo)
{
	int i = 0;
	while (i < packetSize && packet[i] != ':') {
		opcode.push_back(packet[i++]);
	}
	i++; // skip ':'

	data.reserve(packetSize - i);
	while (i < packetSize && packet[i] != 0) {
		data.push_back(packet[i++]);
	}
}
