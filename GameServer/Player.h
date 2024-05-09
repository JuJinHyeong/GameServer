#pragma once
#include "Transform.h"

class Player
{
public:
	Player();
	~Player();

	Transform& GetTransform();
	const Transform& GetTransform() const;

private:
	Transform mTransform;
};

