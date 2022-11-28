#pragma once

class Vector2
{
public:
	Vector2(float x = 0, float y = 0);
	Vector2(const Vector2& v);
	Vector2& operator=(const Vector2& v);
	Vector2& Normalization();
	inline x();
	inline y();
private:
	float x;
	float y;
};

Vector2::x()
{
	return this->x;
}

Vector2::y()
{
	return this->y;
}

const Vector2 operator*(const Vector2& lhs, const Vector2& rhs);