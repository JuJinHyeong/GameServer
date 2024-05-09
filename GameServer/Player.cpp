#include "Player.h"

Player::Player()
{
}

Player::~Player()
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
