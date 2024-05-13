#include "ServerBase.h"
#include <iostream>

ServerBase::ServerBase() {

}

ServerBase::~ServerBase() {
	WSACleanup();
}

bool ServerBase::Initialize()
{
	bool result = InitSocket();
	if (!result) {
		return false;
	}
	result = BindAndListen();
	if (!result) {
		return false;
	}
	return true;
}

bool ServerBase::InitSocket() {
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

bool ServerBase::BindAndListen() {
	SOCKADDR_IN serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
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

bool ServerBase::StartServer() {
	CreateClient();
	mIOCPHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, MAX_WORKER_THREAD_COUNT);
	if (mIOCPHandle == NULL) {
		std::cerr << "CreateIoCompletionPort failed with error: " << GetLastError() << std::endl;
		return false;
	}

	bool result = CreateWorkerThread();
	if (!result) {
		return result;
	}

	result = CreateAcceptThread();
	if (!result) {
		return result;
	}

	result = CreateDispatchThread();
	if (!result) {
		return result;
	}

	std::cout << "Start Server Success\n";
	return true;
}

void ServerBase::DestroyThread() {
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

	mbIsDispatchRun = false;
	for (std::thread& th : mDispatchThreadArr) {
		if (th.joinable()) {
			th.join();
		}
	}
}

void ServerBase::CreateClient() {
	for (UINT32 i = 0; i < MAX_CLIENT; ++i) {
		mClientInfoArr.emplace_back();
	}
}

bool ServerBase::CreateWorkerThread() {
	for (UINT32 i = 0; i < MAX_WORKER_THREAD_COUNT; ++i) {
		mIOWorkerThreadArr.emplace_back([this]() { WorkerThread(); });
	}
	std::cout << "Create Worker Thread Success\n";
	return true;
}

bool ServerBase::CreateAcceptThread() {
	mAcceptThread = std::thread([this]() { AcceptThread(); });
	std::cout << "Create Accept Thread Success\n";
	return true;
}

bool ServerBase::CreateDispatchThread()
{
	for (UINT32 i = 0; i < MAX_DISPATCH_THREAD_COUNT; ++i) {
		mDispatchThreadArr.emplace_back([this]() { DispatchThread(); });
	}
	std::cout << "Create Dispatch Thread Success\n";
	return true;
}

ClientInfo* ServerBase::GetEmptyClientInfo() {
	for (ClientInfo& clientInfo : mClientInfoArr) {
		if (clientInfo.ClientSocket == INVALID_SOCKET) {
			return &clientInfo;
		}
	}
	return nullptr;
}

bool ServerBase::BindIOCompletionPort(ClientInfo* pClientInfo) {
	auto hIOCP = CreateIoCompletionPort(
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

bool ServerBase::BindRecv(ClientInfo* pClientInfo) {
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

bool ServerBase::BindSend(ClientInfo* pClientInfo, const char* message, const int messageLength) {
	DWORD recvNumBytes = 0;
	CopyMemory(pClientInfo->SendOverlapped.Buffer, message, messageLength);
	pClientInfo->SendOverlapped.Buffer[messageLength] = '\0';
	// TODO: compare with allocated memory vs allocating memory
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

void ServerBase::AcceptThread() {
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

		bool result = BindIOCompletionPort(pClientInfo);
		if (!result) {
			return;
		}

		result = BindRecv(pClientInfo);
		if (!result) {
			std::cerr << "Bind Recv failed\n";
			return;
		}

		inet_ntop(AF_INET, &(clientAddr.sin_addr), pClientInfo->IP, INET_ADDRSTRLEN);

		// Do Things when client is connected
		OnConnect(pClientInfo);

		++mClientCount;
	}
}

void ServerBase::WorkerThread() {
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

		// if client is closed or terminated
		if (result == TRUE && IOSize == 0 || (result == FALSE || (IOSize == 0 && lpOverlapped == NULL))) {
			std::cout << "Disconnected: " << (int)pClientInfo->ClientSocket << std::endl;
			CloseSocket(pClientInfo);
			continue;
		}

		// if client sends nothing
		if (IOSize == NULL) {
			continue;
		}

		OverlappedEx* pOverlappedEx = (OverlappedEx*)lpOverlapped;
		if (pOverlappedEx->OperationType == eIOOperation::RECV) {
			OnReceive(pClientInfo, pOverlappedEx->Buffer, IOSize);

			BindRecv(pClientInfo);
		}
		else if (pOverlappedEx->OperationType == eIOOperation::SEND) {
			std::cout << "Send Data: " << pOverlappedEx->Buffer << std::endl;
		}
		else {
			std::cerr << "Unknown Operation Type\n";
		}
	}
}

void ServerBase::DispatchThread()
{
	while (mbIsDispatchRun) {
		// wait unitl message queue is not empty
		Message message = mMessageQueue.Pop();

		std::cout << "Recv Opcode: " << message.opcode << " Data: " << message.data << std::endl;

		BindSend(message.pClientInfo, message.data.c_str(), (int)message.data.size());
	}
}

void ServerBase::CloseSocket(ClientInfo* pClientInfo, bool bIsForce) {
	// Do Things when client is disconnected
	OnClose(pClientInfo);

	LINGER linger = { 0, 0 };

	if (bIsForce) {
		linger.l_onoff = 1;
	}

	shutdown(pClientInfo->ClientSocket, SD_BOTH);

	setsockopt(
		pClientInfo->ClientSocket,
		SOL_SOCKET,
		SO_LINGER,
		(char*)&linger,
		sizeof(linger)
	);

	closesocket(pClientInfo->ClientSocket);

	pClientInfo->ClientSocket = INVALID_SOCKET;
}