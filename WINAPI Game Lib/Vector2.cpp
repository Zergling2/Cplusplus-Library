#include "Vector2.h"
#include <cmath>

Vector2::Vector2(float x, float y) :
	x(x), y(y)
{
}

Vector2::Vector2(const Vector2& v) :
	x(v.x), y(v.y)
{
}

Vector2& Vector2::operator=(const Vector2& v)
{
	this->x = v.x;
	this->y = v.y;

	return *this;
}

Vector2& Vector2::Normalization()
{
	float k = sqrtf(this->x * this->x + this->y * this->y);
	this->x /= k;
	this->y /= k;

	return *this;
}

const Vector2 operator*(const Vector2& lhs, const Vector2& rhs)
{
	return Vector2(lhs.GetX() * rhs.GetX(), lhs.GetY() * rhs.GetY());1
}
