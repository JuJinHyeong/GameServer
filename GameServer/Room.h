#pragma once
#include <vector>
#include "Player.h"

class Room
{
public:
	Room();
	~Room();
private:
	std::vector<Player> mPlayerArr;
};

