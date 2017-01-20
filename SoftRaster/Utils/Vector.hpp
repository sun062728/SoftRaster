#pragma once

#include <cmath>
#include "Matrix.hpp"

class Color4f
{
public:
	float r, g, b, a;

	Color4f() :
		r(0.0f), g(0.0f), b(0.0f), a(0.0f)
	{	}
	Color4f(float _r, float _g, float _b, float _a) :
		r(_r), g(_g), b(_b), a(_a)
	{	}

	Color4f operator+(const Color4f &rhs)
	{
		return Color4f(r + rhs.r, g + rhs.g, b + rhs.b, a + rhs.a);
	}

	Color4f& operator=(const Color4f &rhs)
	{
		r = rhs.r; g = rhs.g; b = rhs.b; a = rhs.a;
		return *this;
	}

	Color4f& operator+=(const Color4f &rhs)
	{
		r += rhs.r; g += rhs.g; b += rhs.b; a += rhs.a;
		return *this;
	}

	Color4f operator*(const float &f)
	{
		return Color4f(r*f, g*f, b*f, a*f);
	}
};

class Vector2f
{
public:
	float x, y;
	Vector2f() :x(0.0f), y(0.0f)
	{	}
	Vector2f(const Vector2f &v) :x(v.x), y(v.y)
	{	}
	Vector2f(float _x, float _y) :
		x(_x), y(_y)
	{	}
	Vector2f& operator=(const Vector2f& rhs)
	{
		x = rhs.x;
		y = rhs.y;
		return *this;
	}
	Vector2f operator*(const float &a)
	{
		Vector2f v(x*a, y*a);
		return v;
	}
	Vector2f operator+(const Vector2f& rhs)
	{
		Vector2f v(x + rhs.x, y + rhs.y);
		return v;
	}
};

class Vector3f
{
public:
	float x, y, z;
	Vector3f() :x(0.0f), y(0.0f), z(0.0f)
	{	}
	Vector3f(const Vector3f &v) :x(v.x), y(v.y), z(v.z)
	{	}
	Vector3f(float _x, float _y, float _z) :
		x(_x), y(_y), z(_z)
	{	}
	float dot(const Vector3f &rhs)const
	{
		return (x*rhs.x + y*rhs.y + z*rhs.z);
	}
	// left-hand coordinate
	Vector3f cross(const Vector3f &rhs)const
	{
		//Vector3f v(y*rhs.z - z*rhs.y, z*rhs.x - x*rhs.z, x*rhs.y - y*rhs.x); // right-hand
		Vector3f v(z*rhs.y - y*rhs.z, x*rhs.z - z*rhs.x, y*rhs.x - x*rhs.y); // left-hand
		return v;
	}
	Vector3f normalize()const
	{
		float len = sqrtf(x*x + y*y + z*z);
		return (*this) / len;
	}
	Vector3f operator*(const Matrix3x3 &m)
	{
		return Vector3f(
			x*m.m11 + y*m.m21 + z*m.m31,
			x*m.m12 + y*m.m22 + z*m.m32,
			x*m.m13 + y*m.m23 + z*m.m33);
	}
	Vector3f& operator=(const Vector3f& rhs)
	{
		x = rhs.x;
		y = rhs.y;
		z = rhs.z;
		return *this;
	}
	Vector3f operator*(const float &a)const
	{
		Vector3f v(x*a, y*a, z*a);
		return v;
	}
	Vector3f operator/(const float &a)const
	{
		float inv = 1.0f / a;
		return (*this)*inv;
	}
	Vector3f operator+(const Vector3f& rhs)const
	{
		Vector3f v(x + rhs.x, y + rhs.y, z + rhs.z);
		return v;
	}
	Vector3f operator-(const Vector3f& rhs)const
	{
		Vector3f v(x - rhs.x, y - rhs.y, z - rhs.z);
		return v;
	}
	Vector3f operator-()const
	{
		return (*this)*(-1.0f);
	}
};

class Vector2i
{
public:
	int x, y;
	Vector2i() :x(0), y(0)
	{	}
	Vector2i(const Vector2i &v) :x(v.x), y(v.y)
	{	}
	Vector2i(int _x, int _y) :
		x(_x), y(_y)
	{	}
	Vector2i& operator=(const Vector2i& rhs)
	{
		x = rhs.x;
		y = rhs.y;
		return *this;
	}
	Vector2i operator*(const int &a)
	{
		Vector2i v(x*a, y*a);
		return v;
	}
	Vector2i operator+(const Vector2i& rhs)
	{
		Vector2i v(x + rhs.x, y + rhs.y);
		return v;
	}
};


class Vector4f
{
public:
	float x, y, z, w;
	Vector4f() :x(0.0f), y(0.0f), z(0.0f), w(0.0f)
	{	}

	Vector4f(const Vector4f &v) :x(v.x), y(v.y), z(v.z), w(v.w)
	{	}

	Vector4f(float _x, float _y, float _z, float _w) :
		x(_x), y(_y), z(_z), w(_w)
	{	}

	Vector4f& operator=(const Vector4f& rhs)
	{
		x = rhs.x;
		y = rhs.y;
		z = rhs.z;
		w = rhs.w;

		return *this;
	}

	Vector4f operator*(const Matrix4x4 &m)
	{
		Vector4f v(
			x*m.m11 + y*m.m21 + z*m.m31 + w*m.m41,
			x*m.m12 + y*m.m22 + z*m.m32 + w*m.m42,
			x*m.m13 + y*m.m23 + z*m.m33 + w*m.m43,
			x*m.m14 + y*m.m24 + z*m.m34 + w*m.m44);

		return v;
	}

	Vector4f operator*(const float &f)
	{
		Vector4f v(x*f, y*f, z*f, w*f);

		return v;
	}

	Vector4f operator+(const Vector4f &rhs)
	{
		Vector4f v(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w);
		return v;
	}
};
