#include <iostream>
#include "IOCompletionPort.h"
using namespace std;

constexpr UINT16 SERVER_PORT = 9000;
constexpr UINT16 MAX_CLIENT = 100;

int main() {
	IOCompletionPort ioCompletionPort;
	ioCompletionPort.InitSocket();
	ioCompletionPort.BindAndListen(SERVER_PORT);
	ioCompletionPort.StartServer(MAX_CLIENT);

	std::cout << "Press any key to exit\n";
	std::cin.get();

	ioCompletionPort.DestroyThread();

	return 0;
}