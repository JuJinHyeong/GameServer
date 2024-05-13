#pragma once
#include <vector>
#include "Player.h"

class Room
{
public:
	Room();
	~Room();

	int AddPlayer(Player player);
	bool IsEmpty();
	bool RemovePlayer(Player player);

private:
	std::vector<Player> mPlayerArr;
};

