#include "Room.h"

Room::Room()
{
}

Room::~Room()
{
}

int Room::AddPlayer(Player player)
{
	mPlayerArr.push_back(player);
	return (int)mPlayerArr.size() - 1;
}

bool Room::IsEmpty()
{
	return mPlayerArr.empty();
}

bool Room::RemovePlayer(Player player)
{
	bool ret = false;
	for (size_t i = 0; i < mPlayerArr.size(); i++) {
		if (mPlayerArr[i].GetClientInfo() == player.GetClientInfo()) {
			mPlayerArr.erase(mPlayerArr.begin() + i);
			ret = true;
			break;
		}
	}
	return ret;
}
