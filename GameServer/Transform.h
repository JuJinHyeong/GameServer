#pragma once
#include "Position.h"

class Transform
{
public:
	Transform();
	Transform(float x, float y);
	~Transform();

	Position& GetPos();
	const Position& GetPos() const;

	void SetPos(float x, float y);
private:
	Position mPos;
};

