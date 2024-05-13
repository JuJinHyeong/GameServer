#include <iostream>
#include "ShootingGameServer.h"
using namespace std;

int main() {
	ShootingGameServer gameServer;
	gameServer.Initialize();
	gameServer.StartServer();

	std::cout << "Press any key to exit\n";
	std::cin.get();

	gameServer.DestroyThread();

	return 0;
}