#include "IOCompletionPort.h"
#include "ClientInfo.h"
#include <iostream>

IOCompletionPort::IOCompletionPort(std::vector<ClientInfo>& clientInfoArr, ThreadSafeQueue<Message>& messageQueue)
	:
	mMessageQueue(messageQueue),
	mClientInfoArr(clientInfoArr)
{
}

IOCompletionPort::~IOCompletionPort()
{
}

bool IOCompletionPort::InitSocket()
{
	// startup winsocket
	WSADATA wsaData;
	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0) {
		std::cerr << "WSAStartup failed with error: " << result << std::endl;
		return false;
	}

	// Create a socket with wsasocket
	// not socket() because socket can use both linux and windows.
	// WSASocket is windows specific
	mListenSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (mListenSocket == INVALID_SOCKET) {
		std::cerr << "WSASocket failed with error: " << WSAGetLastError() << std::endl;
		return false;
	}

	std::cout << "Socket Initialized Success\n";
	return true;
}

bool IOCompletionPort::BindAndListen(const UINT16 serverPort)
{
	SOCKADDR_IN serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(serverPort);
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	int result = bind(mListenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
	if (result == SOCKET_ERROR) {
		std::cerr << "bind failed with error: " << WSAGetLastError() << std::endl;
		return false;
	}

	// bind	iocompletionport, set listen queue to 5
	result = listen(mListenSocket, 5);
	if (result == SOCKET_ERROR) {
		std::cerr << "listen failed with error: " << WSAGetLastError() << std::endl;
		return false;
	}

	std::cout << "Bind and Listen Success\n";
	return true;
}

bool IOCompletionPort::Create()
{
	mIOCPHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, MAX_WORKER_THREAD_COUNT);
	if (mIOCPHandle == NULL) {
		std::cerr << "CreateIoCompletionPort failed with error: " << GetLastError() << std::endl;
		return false;
	}
	return true;
}

bool IOCompletionPort::BindWithClient(ClientInfo* pClientInfo)
{
	HANDLE hIOCP = CreateIoCompletionPort(
		(HANDLE)pClientInfo->ClientSocket,
		mIOCPHandle,
		(ULONG_PTR)pClientInfo,
		0
	);

	if (hIOCP == NULL || mIOCPHandle == NULL) {
		std::cerr << "CreateIoCompletionPort failed with error: " << GetLastError() << std::endl;
		return false;
	}

	return true;
}

bool IOCompletionPort::CreateWorkerThread()
{
	for (UINT32 i = 0; i < MAX_WORKER_THREAD_COUNT; ++i) {
		mIOWorkerThreadArr.emplace_back([this]() { WorkerThread(); });
	}
	std::cout << "Create Worker Thread Success\n";
	return true;
}

bool IOCompletionPort::CreateAcceptThread()
{
	mAcceptThread = std::thread([this]() { AcceptThread(); });
	std::cout << "Create Accept Thread Success\n";
	return true;
}

void IOCompletionPort::DestoryThread()
{
	mbIsAcceptRun = false;
	closesocket(mListenSocket);
	if (mAcceptThread.joinable()) {
		mAcceptThread.join();
	}

	mbIsWorkerRun = false;
	CloseHandle(mIOCPHandle);
	for (std::thread& th : mIOWorkerThreadArr) {
		if (th.joinable()) {
			th.join();
		}
	}
}

bool IOCompletionPort::BindRecv(ClientInfo* pClientInfo)
{
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

bool IOCompletionPort::BindSend(ClientInfo* pClientInfo, const char* message, const int messageLength)
{
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

ClientInfo* IOCompletionPort::GetEmptyClientInfo()
{
	for (ClientInfo& clientInfo : mClientInfoArr) {
		if (clientInfo.ClientSocket == INVALID_SOCKET) {
			return &clientInfo;
		}
	}
	return nullptr;
}

void IOCompletionPort::AcceptThread()
{
	SOCKADDR_IN clientAddr;
	int clientAddrSize = sizeof(clientAddr);
	while (mbIsAcceptRun) {
		ClientInfo* pClientInfo = GetEmptyClientInfo();
		if (pClientInfo == nullptr) {
			std::cerr << "Client is full\n";
			return;
		}

		pClientInfo->ClientSocket = accept(mListenSocket, (SOCKADDR*)&clientAddr, &clientAddrSize);
		if (pClientInfo->ClientSocket == INVALID_SOCKET) {
			std::cerr << "accept failed with error: " << WSAGetLastError() << std::endl;
			return;
		}

		bool result = BindWithClient(pClientInfo);
		if (!result) {
			return;
		}

		result = BindRecv(pClientInfo);
		if (!result) {
			return;
		}

		char clientIP[INET_ADDRSTRLEN] = { 0 };
		inet_ntop(AF_INET, &(clientAddr.sin_addr), clientIP, INET_ADDRSTRLEN);
		std::cout << "Client Connected: " << clientIP << std::endl;
	}
}

void IOCompletionPort::WorkerThread()
{
	ClientInfo* pClientInfo = nullptr;
	DWORD IOSize = 0;

	LPOVERLAPPED lpOverlapped = nullptr;

	while (mbIsWorkerRun) {
		BOOL result = GetQueuedCompletionStatus(
			mIOCPHandle,
			&IOSize,
			(PULONG_PTR)&pClientInfo,
			&lpOverlapped,
			INFINITE
		);

		// if client is closed connection
		if (result == TRUE && IOSize == 0) {
			std::cout << "Disconnected: " << (int)pClientInfo->ClientSocket << std::endl;
			pClientInfo->CloseSocket();
			continue;
		}

		// if client sends nothing
		if (IOSize == NULL) {
			continue;
		}

		// if client is terminated
		if (result == FALSE || (IOSize == 0 && lpOverlapped == NULL)) {
			std::cout << "Disconnected: " << (int)pClientInfo->ClientSocket << std::endl;
			pClientInfo->CloseSocket();
			continue;
		}

		OverlappedEx* pOverlappedEx = (OverlappedEx*)lpOverlapped;
		if (pOverlappedEx->OperationType == eIOOperation::RECV) {
			 //convert packet to message
			pOverlappedEx->Buffer[IOSize] = '\0';
			Message message(pOverlappedEx->Buffer, pClientInfo);
			mMessageQueue.Push(message);

			BindRecv(pClientInfo);
		}
		else if (pOverlappedEx->OperationType == eIOOperation::SEND) {
			std::cout << "Send Data: " << pOverlappedEx->Buffer << '\n';
		}
		else {
			std::cerr << "Unknown Operation Type\n";
		}
	}
}
