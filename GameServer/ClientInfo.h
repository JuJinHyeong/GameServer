#pragma once
#include <WinSock2.h>

constexpr UINT32 MAX_SOCKET_BUFFER_SIZE = 1024;

enum eIOOperation {
	RECV,
	SEND
};

struct OverlappedEx {
	WSAOVERLAPPED Overlapped;
	SOCKET ClientSocket;
	WSABUF WSABuf;
	char Buffer[MAX_SOCKET_BUFFER_SIZE];
	enum eIOOperation OperationType;
};

class ClientInfo
{
public:
	void CloseSocket(bool bIsForce = false);
public:
	SOCKET ClientSocket = INVALID_SOCKET;
	OverlappedEx RecvOverlapped = { 0 };
	OverlappedEx SendOverlapped = { 0 };
};

