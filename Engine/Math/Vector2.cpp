#include "Vector2.h"

namespace Craft
{
	// 전역변수 초기화
	Vector2 Vector2::Zero(0.0f, 0.0f);
	Vector2 Vector2::One(1.0f, 1.0f);
	Vector2 Vector2::Right(1.0f, 0.0f);
	Vector2 Vector2::Up(0.0f, 1.0f);

	Vector2::Vector2(float x, float y)
		: x(x), y(y)
	{

	}

	Vector2 Vector2::operator+(const Vector2& other) const
	{
		return Vector2(x + other.x, y + other.y);
	}

	Vector2 Vector2::operator-(const Vector2& other) const
	{
		return Vector2(x - other.x, y - other.y);
	}

	Vector2 Vector2::operator*(const Vector2& other) const
	{
		return Vector2(x * other.x, y * other.y);
	}
	
	Vector2 Vector2::operator*(float scale) const
	{
		return Vector2(x * scale, y * scale);
	}

	bool Vector2::operator==(const Vector2& other) const
	{
		return (x == other.x) && (y == other.y);
	}
	
	bool Vector2::operator!=(const Vector2& other) const
	{
		return !(*this == other);
	}
}