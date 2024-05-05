#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")

#define SERVER_ADDRESS "127.0.0.1"
#define SERVER_PORT 9000
#define BUFFER_SIZE 1024

int main() {
    WSADATA wsaData;
    SOCKET sock = INVALID_SOCKET;
    sockaddr_in serverAddr;
    char sendBuffer[BUFFER_SIZE], recvBuffer[BUFFER_SIZE];

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed!" << std::endl;
        return 1;
    }

    // Create a socket
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        std::cerr << "Socket creation failed!" << std::endl;
        WSACleanup();
        return 1;
    }

    // Set up the server address
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_ADDRESS, &serverAddr.sin_addr);

    // Connect to the server
    if (connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Failed to connect to server!" << std::endl;
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    std::cout << "Connected to server. Type messages to send. Type 'quit' to exit." << std::endl;

    // Communication loop
    while (true) {
        std::cout << "You: ";
        std::cin.getline(sendBuffer, BUFFER_SIZE);

        // Check for "quit" command
        if (strcmp(sendBuffer, "quit") == 0) {
            std::cout << "Exiting..." << std::endl;
            break;
        }

        // Send message to the server
        if (send(sock, sendBuffer, strlen(sendBuffer), 0) == SOCKET_ERROR) {
            std::cerr << "Send failed!" << std::endl;
            break;
        }

        // Receive echo from server
        int recvSize = recv(sock, recvBuffer, BUFFER_SIZE, 0);
        if (recvSize == SOCKET_ERROR || recvSize == 0) {
            std::cerr << "Receive failed!" << std::endl;
            break;
        }

        recvBuffer[recvSize] = '\0';
        std::cout << "Server: " << recvBuffer << std::endl;
    }

    // Cleanup
    closesocket(sock);
    WSACleanup();
    return 0;
}