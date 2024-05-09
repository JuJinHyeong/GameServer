#include "ClientInfo.h"

void ClientInfo::CloseSocket(bool bIsForce)
{
	LINGER linger = { 0, 0 };

	if (bIsForce) {
		linger.l_onoff = 1;
	}

	shutdown(this->ClientSocket, SD_BOTH);

	setsockopt(
		this->ClientSocket,
		SOL_SOCKET,
		SO_LINGER,
		(char*)&linger,
		sizeof(linger)
	);

	closesocket(this->ClientSocket);

	this->ClientSocket = INVALID_SOCKET;
}
