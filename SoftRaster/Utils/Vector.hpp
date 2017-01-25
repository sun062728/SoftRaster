#pragma once

/*
** Vector2i: window coord
** Vector2f: tex coord
** Vector3f: pos, normal, tangent...
** Vector4f: pos
*/

#include <cmath>
#include "Matrix.hpp"

struct Vector2f
{
	float x, y;
	Vector2f() = default;
	Vector2f(Vector2f const &v) :x(v.x), y(v.y) {}
	Vector2f(float _x, float _y) :x(_x), y(_y) {}
	Vector2f& operator=(Vector2f const &rhs) 
	{
		x = rhs.x;
		y = rhs.y;
		return *this;
	}
	Vector2f& operator*=(float f)
	{
		x *= f; y *= f;
		return *this;
	}
	Vector2f operator*(float f)
	{
		Vector2f v(*this);
		return v *= f;
	}
	Vector2f& operator+=(Vector2f const &r)
	{
		x += r.x; y += r.y;
		return *this;
	}
	Vector2f operator+(Vector2f const &r)
	{
		Vector2f v(*this);
		return v += r;
	}
	Vector2f operator-(int) 
	{
		return Vector2f(-x, -y);
	}
};

struct Vector3f
{
	float x, y, z;
	Vector3f() = default;
	Vector3f(const Vector3f &v) :x(v.x), y(v.y), z(v.z)	{}
	Vector3f(float _x, float _y, float _z) :x(_x), y(_y), z(_z)	{}
	Vector3f& operator=(Vector3f const &rhs)
	{
		x = rhs.x;
		y = rhs.y;
		z = rhs.z;
		return *this;
	}
	float dot(Vector3f const &rhs)const
	{
		return (x*rhs.x + y*rhs.y + z*rhs.z);
	}
	// left-hand coordinate
	Vector3f cross(Vector3f const &rhs)const
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
	Vector3f operator*(Matrix3x3 const &m)
	{
		return Vector3f(
			x*m.m11 + y*m.m21 + z*m.m31,
			x*m.m12 + y*m.m22 + z*m.m32,
			x*m.m13 + y*m.m23 + z*m.m33);
	}
	Vector3f operator*(float a)const
	{
		Vector3f v(x*a, y*a, z*a);
		return v;
	}
	Vector3f operator/(float a)const
	{
		float inv = 1.0f / a;
		return (*this)*inv;
	}
	Vector3f operator+(Vector3f const &rhs)const
	{
		Vector3f v(x + rhs.x, y + rhs.y, z + rhs.z);
		return v;
	}
	Vector3f operator-(Vector3f const &rhs)const
	{
		Vector3f v(x - rhs.x, y - rhs.y, z - rhs.z);
		return v;
	}
	Vector3f operator-()const 
	{
		return (*this)*(-1.0f);
	}
};

struct Vector2i
{
	int x, y;
	Vector2i() = default;
	Vector2i(const Vector2i &v) :x(v.x), y(v.y) {}
	Vector2i(int _x, int _y) :x(_x), y(_y) {}
	Vector2i& operator=(Vector2i const &rhs) 
	{
		x = rhs.x; y = rhs.y;
		return *this;
	}
	Vector2i operator*(int a)const 
	{
		Vector2i v(x*a, y*a);
		return v;
	}
	Vector2i operator+(Vector2i const &rhs)const
	{
		Vector2i v(x + rhs.x, y + rhs.y);
		return v;
	}
};

struct Vector4f
{
	union {
		struct
		{
			float x, y, z, w;
		};
		struct 
		{
			float r, g, b, a;
		};
	};
	Vector4f() = default;
	Vector4f(Vector4f const &v) :x(v.x), y(v.y), z(v.z), w(v.w) {}
	Vector4f(float _x, float _y, float _z, float _w) :x(_x), y(_y), z(_z), w(_w) {}
	Vector4f& operator=(Vector4f const &rhs) 
	{
		x = rhs.x; y = rhs.y; z = rhs.z; w = rhs.w;
		return *this;
	}
	Vector4f operator*(Matrix4x4 const &m)const 
	{
		Vector4f v(
			x*m.m11 + y*m.m21 + z*m.m31 + w*m.m41,
			x*m.m12 + y*m.m22 + z*m.m32 + w*m.m42,
			x*m.m13 + y*m.m23 + z*m.m33 + w*m.m43,
			x*m.m14 + y*m.m24 + z*m.m34 + w*m.m44);
		return v;
	}
	Vector4f operator*(float f)const
	{
		Vector4f v(x*f, y*f, z*f, w*f);
		return v;
	}
	Vector4f operator+=(Vector4f const &rhs)
	{
		x += rhs.x;	y += rhs.y; z += rhs.z; w += rhs.w;
		return *this;
	}
	Vector4f operator+(Vector4f const &rhs) const
	{
		Vector4f v(*this);
		v += rhs;
		return v;
	}
};

typedef Vector4f Color4f;