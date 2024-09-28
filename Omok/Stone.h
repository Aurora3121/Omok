#pragma once
class Stone
{
public:
	enum Color		// ¹ÙµÏµ¹ »ö»ó
	{
		BLACK = 0,
		WHITE = 1
	};

private:
	Vector2Int		_pos;	// ÁÂÇ¥
	Stone::Color	_color;	// »ö»ó

public:
	Stone(Stone::Color color, const Vector2Int& pos) : _color(color), _pos(pos) {};
	~Stone() = default;

	const Vector2Int& GetPos() { return _pos; }
	Stone::Color GetColor() { return _color; }
};

