#include "Player.h"
#include "ClientInfo.h"

Player::Player()
{
}

Player::~Player()
{
}

Player::Player(ClientInfo* pClientInfo)
	:
	mpClientInfo(pClientInfo)
{
}

Transform& Player::GetTransform()
{
	return mTransform;
}

const Transform& Player::GetTransform() const
{
	return mTransform;
}

const ClientInfo* const Player::GetClientInfo() const
{
	return mpClientInfo;
}
