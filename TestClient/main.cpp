#pragma comment(lib, "Ws2_32.lib")
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <conio.h> // Windows에서 키보드 입력을 위해 사용
#include <windows.h>
#include <string>
#include <sstream>

#define SERVER_ADDRESS "127.0.0.1"
#define SERVER_PORT 9000
#define BUFFER_SIZE 1024

const int n = 5; // 그리드 크기
int playerX = 0, playerY = 0;

void clearConsole() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    DWORD consoleSize = csbi.dwSize.X * csbi.dwSize.Y;
    DWORD charsWritten;
    FillConsoleOutputCharacter(hConsole, ' ', consoleSize, { 0, 0 }, &charsWritten);
    FillConsoleOutputAttribute(hConsole, csbi.wAttributes, consoleSize, { 0, 0 }, &charsWritten);
    SetConsoleCursorPosition(hConsole, { 0, 0 });
}

void drawGrid() {
    clearConsole();
    for (int y = 0; y < n; ++y) {
        for (int x = 0; x < n; ++x) {
            if (x == playerX && y == playerY)
                std::cout << '.';
            else
                std::cout << '#';
        }
        std::cout << '\n';
    }
}

void updatePlayerPosition(char input) {
    switch (input) {
    case 'w': // 위로 이동
        if (playerY > 0) --playerY;
        break;
    case 's': // 아래로 이동
        if (playerY < n - 1) ++playerY;
        break;
    case 'a': // 왼쪽으로 이동
        if (playerX > 0) --playerX;
        break;
    case 'd': // 오른쪽으로 이동
        if (playerX < n - 1) ++playerX;
        break;
    }
}

int main() {
    WSADATA wsaData;
    SOCKET sock = INVALID_SOCKET;
    sockaddr_in serverAddr;
    char sendBuffer[BUFFER_SIZE] = { 0 }, recvBuffer[BUFFER_SIZE] = { 0 };
    
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

    while(true) {
        std::string message;
		std::getline(std::cin, message);
		if (message == "quit") break;
        if (send(sock, message.c_str(), (int)message.size(), 0) == SOCKET_ERROR) {
			std::cerr << "Send failed!" << std::endl;
			break;
		}
		int recvSize = recv(sock, recvBuffer, BUFFER_SIZE, 0);
        if (recvSize == SOCKET_ERROR || recvSize == 0) {
			std::cerr << "Receive failed!" << std::endl;
			break;
		}
		recvBuffer[recvSize] = '\0';
		std::cout << "Server: " << recvBuffer << std::endl;
    }

    //std::stringstream ss;
    //drawGrid();
    //while (true) {
    //    if (_kbhit()) { // 키보드 입력이 있을 때만 처리
    //        char input = _getch(); // 키보드 입력을 받음
    //        updatePlayerPosition(input); // 플레이어의 위치 업데이트

    //        // Send message to the server
    //        ss.str("");
    //        ss << playerX << ',' << playerY;
    //        std::string message = ss.str();
    //        if (send(sock, message.c_str(), (int)message.size() , 0) == SOCKET_ERROR) {
    //            std::cerr << "Send failed!" << std::endl;
    //            break;
    //        }
    //        
    //        // Receive echo from server
    //        int recvSize = recv(sock, recvBuffer, BUFFER_SIZE, 0);
    //        if (recvSize == SOCKET_ERROR || recvSize == 0) {
    //            std::cerr << "Receive failed!" << std::endl;
    //            break;
    //        }
    //        
    //        recvBuffer[recvSize] = '\0';
    //        std::cout << "Server: " << recvBuffer << std::endl;

    //        drawGrid();
    //    }
    //}

    closesocket(sock);
    WSACleanup();
    return 0;
}