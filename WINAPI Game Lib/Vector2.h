#pragma once

class Vector2
{
public:
	inline Vector2(float x = 0, float y = 0);
	inline Vector2(const Vector2& v);
	inline Vector2& operator=(const Vector2& v);
	inline Vector2& Normalization();
	inline const float GetX();
	inline const float GetY();
private:
	float x;
	float y;
};

inline Vector2::Vector2(float x, float y) :
	x(x), y(y)
{
}

inline Vector2::Vector2(const Vector2& v) :
	x(v.x), y(v.y)
{
}

inline Vector2& Vector2::operator=(const Vector2& v)
{
	this->x = v.x;
	this->y = v.y;

	return *this;
}

inline Vector2& Vector2::Normalization()
{
	float k = sqrtf(this->x * this->x + this->y * this->y);
	this->x /= k;
	this->y /= k;

	return *this;
}


inline const float Vector2::GetX()
{
	return this->x;
}

inline const float Vector2::GetY()
{
	return this->y;
}

inline const Vector2 operator*(const Vector2& lhs, const Vector2& rhs)
{
	return Vector2(lhs.GetX() * rhs.GetX(), lhs.GetY() * rhs.GetY());
}