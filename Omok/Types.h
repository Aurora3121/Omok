#pragma once
class Vector2Int
{
public:
	int x;
	int y;

	Vector2Int(int x, int y) : x(x), y(y) {};
	~Vector2Int() = default;

	Vector2Int& operator*(int num)
	{
		x *= num;
		y *= num;
		return *this;
	}
};