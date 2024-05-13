#pragma once
#include "Transform.h"

class ClientInfo;

class Player
{
public:
	Player();
	~Player();
	Player(ClientInfo* pClientInfo);

	Transform& GetTransform();
	const Transform& GetTransform() const;

	const ClientInfo* const GetClientInfo() const;

private:
	Transform mTransform;
	ClientInfo* mpClientInfo = nullptr;
};

