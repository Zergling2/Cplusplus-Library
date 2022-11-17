#pragma once

class Vector2
{
public:
	Vector2(float x = 0, float y = 0);
	Vector2(const Vector2& v);
	Vector2& operator=(const Vector2& v);
	Vector2& Normalization();
	float x;
	float y;
};

const Vector2 operator*(const Vector2& a, const Vector2& b);