#pragma comment(lib, "ws2_32.lib")
#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>

constexpr UINT16 SERVER_PORT = 9000;
constexpr const char* SERVER_IP = "127.0.0.1";
constexpr int BUFFER_SIZE = 256;

int main() {
	std::ios_base::sync_with_stdio(false);
	std::cin.tie(nullptr); std::cout.tie(nullptr);

	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != NULL) {
		std::cerr << "WSAStartup failed" << std::endl;
		return -1;
	}

	SOCKET hSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (hSocket == INVALID_SOCKET) {
		std::cerr << "socket failed" << std::endl;
		return -1;
	}

	std::cout << "Socket Initialized\n";

	SOCKADDR_IN serverAddress;
	char outputBuffer[256] = { 0 };
	char outputMessage[256] = { 0 };
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(SERVER_PORT);
	inet_pton(AF_INET, "127.0.0.1", &serverAddress.sin_addr.s_addr);
	
	int result = connect(hSocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress));
	if (result == SOCKET_ERROR) {
		std::cerr << "connect error " << WSAGetLastError() << std::endl;
		return -1;
	}

	std::cout << "Connection Success\n";
	while (true) {
		std::cout << "Input Message: ";
		std::cin >> outputMessage;
		if (outputMessage[0] == 'q' && outputMessage[1] == '\0') {
			break;
		}
		int sendLength = send(hSocket, outputMessage, strlen(outputMessage), 0);
		if (sendLength == SOCKET_ERROR) {
			std::cerr << "send error " << WSAGetLastError() << std::endl;
			closesocket(hSocket);
			return -1;
		}

		int recvLength = recv(hSocket, outputBuffer, sizeof(outputBuffer), 0);
		if (recvLength == SOCKET_ERROR) {
			std::cerr << "recv error " << WSAGetLastError() << std::endl;
			closesocket(hSocket);
			return -1;
		}
		if (recvLength < BUFFER_SIZE) {
			outputBuffer[recvLength] = 0;
			std::cout << "Received Message: " << outputBuffer << std::endl;
		}
		else {
			std::cerr << "Buffer Overflow" << std::endl;
		}
	}

	closesocket(hSocket);
	std::cout << "Client Terminated" << std::endl;

	return 0;
}