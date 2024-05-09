#pragma once
class Position
{
public:
	Position();
	Position(float x, float y);
	~Position();

	void SetPos(float x, float y);
private:
	float mX = 0.f, mY = 0.f;
};

