#include <iostream>
#include "GameServer.h"
using namespace std;

int main() {
	GameServer gameServer;
	gameServer.InitSocket();
	gameServer.BindAndListen();
	gameServer.StartServer();

	std::cout << "Press any key to exit\n";
	std::cin.get();

	gameServer.DestroyThread();

	return 0;
}