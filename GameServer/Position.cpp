#include "Position.h"

Position::Position()
{
}

Position::Position(float x, float y)
	:
	mX(x),
	mY(y)
{
}

Position::~Position()
{
}

void Position::SetPos(float x, float y)
{
	mX = x;
	mY = y;
}
