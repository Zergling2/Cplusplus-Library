#pragma once

class Vector2
{
public:
	Vector2(float x = 0, float y = 0);
	Vector2(const Vector2& v);
	Vector2& operator=(const Vector2& v);
	Vector2& Normalization();
	inline const float GetX();
	inline const float GetY();
private:
	float x;
	float y;
};

const float Vector2::GetX()
{
	return this->x;
}

const float Vector2::GetY()
{
	return this->y;
}

const Vector2 operator*(const Vector2& lhs, const Vector2& rhs);