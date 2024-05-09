#include "Transform.h"

Transform::Transform()
	:
	mPos()
{
}

Transform::Transform(float x, float y)
	:
	mPos(x, y)
{
}

Transform::~Transform()
{
}

Position& Transform::GetPos()
{
	return mPos;
}

const Position& Transform::GetPos() const
{
	return mPos;
}

void Transform::SetPos(float x, float y)
{
	mPos.SetPos(x, y);
}
