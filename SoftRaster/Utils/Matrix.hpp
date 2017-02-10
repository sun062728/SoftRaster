/*
** Matrix3x3 float
** Matrix4x4 float
** 
** Matrix2x2 float is for internal use
*/

#pragma once

#include <cmath>

#define PI 3.14159265358979323846

struct Matrix2x2
{
	float m11, m12;
	float m21, m22;

	Matrix2x2() = default;

	Matrix2x2(float _m11, float _m12, float _m21, float _m22)
		:m11(_m11), m12(_m12),	m21(_m21), m22(_m22) {}

	Matrix2x2 Matrix2x2::operator*(const Matrix2x2 &rhs)const
	{
		Matrix2x2 m;
		m.m11 = m11*rhs.m11 + m12*rhs.m21;
		m.m12 = m11*rhs.m12 + m12*rhs.m22;
		m.m21 = m21*rhs.m11 + m22*rhs.m21;
		m.m22 = m21*rhs.m12 + m22*rhs.m22;
		return m;
	}

	Matrix2x2 Matrix2x2::operator*(float rhs)const 
	{
		return Matrix2x2(m11*rhs, m12*rhs, m21*rhs, m22*rhs);
	}

	Matrix2x2 Matrix2x2::operator/(float rhs)const 
	{
		float inv = 1.0f / rhs;
		return (*this)*inv;
	}

	Matrix2x2 Matrix2x2::Inv()const 
	{
		Matrix2x2 m(m22, -m12, -m21, m22);
		float absDet = fabsf(det());
		m = m / absDet;

		return m;
	}

	float det()const
	{
		return m11*m22 - m12*m21;
	}

	void clear()
	{
		m11 = m12 =
		m21 = m22 = 0.0f;
	}

	void identity()
	{
		clear();
		m11 = m22 = 1.0f;
	}
};

struct Matrix3x3
{
	float m11, m12, m13;
	float m21, m22, m23;
	float m31, m32, m33;

	Matrix3x3() = default;

	Matrix3x3(
		float _m11, float _m12, float _m13,
		float _m21, float _m22, float _m23,
		float _m31, float _m32, float _m33) :
		m11(_m11), m12(_m12), m13(_m13),
		m21(_m21), m22(_m22), m23(_m23),
		m31(_m31), m32(_m32), m33(_m33)
	{	}

	Matrix3x3 Matrix3x3::operator*(const Matrix3x3 &rhs)const
	{
		Matrix3x3 m;

		m.m11 = m11*rhs.m11 + m12*rhs.m21 + m13*rhs.m31;
		m.m12 = m11*rhs.m12 + m12*rhs.m22 + m13*rhs.m32;
		m.m13 = m11*rhs.m13 + m12*rhs.m23 + m13*rhs.m33;

		m.m21 = m21*rhs.m11 + m22*rhs.m21 + m23*rhs.m31;
		m.m22 = m21*rhs.m12 + m22*rhs.m22 + m23*rhs.m32;
		m.m23 = m21*rhs.m13 + m22*rhs.m23 + m23*rhs.m33;

		m.m31 = m31*rhs.m11 + m32*rhs.m21 + m33*rhs.m31;
		m.m32 = m31*rhs.m12 + m32*rhs.m22 + m33*rhs.m32;
		m.m33 = m31*rhs.m13 + m32*rhs.m23 + m33*rhs.m33;

		return m;
	}

	Matrix3x3 Matrix3x3::operator*(float rhs)const
	{
		return Matrix3x3(
			m11*rhs, m12*rhs, m13*rhs,
			m21*rhs, m22*rhs, m23*rhs,
			m31*rhs, m32*rhs, m33*rhs);
	}

	Matrix3x3 Matrix3x3::operator/(const float &rhs)const
	{
		return (*this)*(1.0f / rhs);
	}

	Matrix3x3 Matrix3x3::transpose()const
	{
		return Matrix3x3(
			m11, m21, m31,
			m12, m22, m32,
			m13, m23, m33);
	}

	Matrix3x3 Matrix3x3::Inv()const
	{
		Matrix2x2 M11(m22, m23, m32, m33);
		Matrix2x2 M12(m21, m23, m31, m33);
		Matrix2x2 M13(m21, m22, m31, m32);
		Matrix2x2 M21(m12, m13, m31, m33);
		Matrix2x2 M22(m11, m13, m31, m33);
		Matrix2x2 M23(m11, m12, m31, m32);
		Matrix2x2 M31(m12, m13, m22, m23);
		Matrix2x2 M32(m11, m13, m21, m23);
		Matrix2x2 M33(m11, m12, m21, m22);

		Matrix3x3 m(
			M11.det(), -M21.det(), M31.det(),
			-M12.det(), M22.det(), -M32.det(),
			M13.det(), -M23.det(), M33.det());

		m = m / (fabsf(this->det()));

		return m;
	}

	float det()const
	{
		return m11*m22*m33 + m21*m32*m13 + m31*m12*m23
			- m13*m22*m31 - m23*m32*m11 - m33*m12*m21;
	}

	void clear()
	{
		m11 = m12 = m13 =
		m21 = m22 = m23 =
		m31 = m32 = m33 = 0.0f;
	}

	void identity()
	{
		clear();
		m11 = m22 = m33 = 1.0f;
	}
};

struct Matrix4x4
{
	float m11, m12, m13, m14;
	float m21, m22, m23, m24;
	float m31, m32, m33, m34;
	float m41, m42, m43, m44;

	Matrix4x4() = default;

	Matrix4x4(
		float _m11, float _m12, float _m13, float _m14,
		float _m21, float _m22, float _m23, float _m24,
		float _m31, float _m32, float _m33, float _m34,
		float _m41, float _m42, float _m43, float _m44) :
		m11(_m11), m12(_m12), m13(_m13), m14(_m14),
		m21(_m21), m22(_m22), m23(_m23), m24(_m24),
		m31(_m31), m32(_m32), m33(_m33), m34(_m34),
		m41(_m41), m42(_m42), m43(_m43), m44(_m44)
	{	}

	Matrix4x4 Matrix4x4::operator*(Matrix4x4 const &lhs)const
	{
		Matrix4x4 m;

		m.m11 = m11*lhs.m11 + m12*lhs.m21 + m13*lhs.m31 + m14*lhs.m41;
		m.m12 = m11*lhs.m12 + m12*lhs.m22 + m13*lhs.m32 + m14*lhs.m42;
		m.m13 = m11*lhs.m13 + m12*lhs.m23 + m13*lhs.m33 + m14*lhs.m43;
		m.m14 = m11*lhs.m14 + m12*lhs.m24 + m13*lhs.m34 + m14*lhs.m44;

		m.m21 = m21*lhs.m11 + m22*lhs.m21 + m23*lhs.m31 + m24*lhs.m41;
		m.m22 = m21*lhs.m12 + m22*lhs.m22 + m23*lhs.m32 + m24*lhs.m42;
		m.m23 = m21*lhs.m13 + m22*lhs.m23 + m23*lhs.m33 + m24*lhs.m43;
		m.m24 = m21*lhs.m14 + m22*lhs.m24 + m23*lhs.m34 + m24*lhs.m44;

		m.m31 = m31*lhs.m11 + m32*lhs.m21 + m33*lhs.m31 + m34*lhs.m41;
		m.m32 = m31*lhs.m12 + m32*lhs.m22 + m33*lhs.m32 + m34*lhs.m42;
		m.m33 = m31*lhs.m13 + m32*lhs.m23 + m33*lhs.m33 + m34*lhs.m43;
		m.m34 = m31*lhs.m14 + m32*lhs.m24 + m33*lhs.m34 + m34*lhs.m44;

		m.m41 = m41*lhs.m11 + m42*lhs.m21 + m43*lhs.m31 + m44*lhs.m41;
		m.m42 = m41*lhs.m12 + m42*lhs.m22 + m43*lhs.m32 + m44*lhs.m42;
		m.m43 = m41*lhs.m13 + m42*lhs.m23 + m43*lhs.m33 + m44*lhs.m43;
		m.m44 = m41*lhs.m14 + m42*lhs.m24 + m43*lhs.m34 + m44*lhs.m44;

		return m;
	}

	Matrix4x4 Matrix4x4::operator*(float rhs)const 
	{
		return Matrix4x4(
			m11*rhs, m12*rhs, m13*rhs, m14*rhs,
			m21*rhs, m22*rhs, m23*rhs, m24*rhs,
			m31*rhs, m32*rhs, m33*rhs, m34*rhs,
			m41*rhs, m42*rhs, m43*rhs, m44*rhs);
	}

	Matrix4x4 Matrix4x4::operator/(float rhs)const 
	{
		return (*this)*(1.0f / rhs);
	}

	Matrix4x4 Matrix4x4::transpose()const 
	{
		return Matrix4x4(
			m11, m21, m31, m41,
			m12, m22, m32, m42,
			m13, m23, m33, m43,
			m14, m24, m34, m44);
	}

	Matrix4x4 Matrix4x4::Inv()const
	{
		Matrix3x3 M11(
			m22, m23, m24,
			m32, m33, m34,
			m42, m43, m44);
		Matrix3x3 M12(
			m21, m23, m24,
			m31, m33, m34,
			m41, m43, m44);
		Matrix3x3 M13(
			m21, m22, m24,
			m31, m32, m34,
			m41, m42, m44);
		Matrix3x3 M14(
			m21, m22, m23,
			m31, m32, m33,
			m41, m42, m43);
		Matrix3x3 M21(
			m12, m13, m14,
			m32, m33, m34,
			m42, m43, m44);
		Matrix3x3 M22(
			m11, m13, m14,
			m31, m33, m34,
			m41, m43, m44);
		Matrix3x3 M23(
			m11, m12, m14,
			m31, m32, m34,
			m41, m42, m44);
		Matrix3x3 M24(
			m11, m12, m13,
			m31, m32, m33,
			m41, m42, m43);
		Matrix3x3 M31(
			m12, m13, m14,
			m22, m23, m24,
			m42, m43, m44);
		Matrix3x3 M32(
			m11, m13, m14,
			m21, m23, m24,
			m41, m43, m44);
		Matrix3x3 M33(
			m11, m12, m14,
			m21, m22, m24,
			m41, m42, m44);
		Matrix3x3 M34(
			m11, m12, m13,
			m21, m22, m23,
			m41, m42, m43);
		Matrix3x3 M41(
			m12, m13, m14,
			m22, m23, m24,
			m32, m33, m34);
		Matrix3x3 M42(
			m11, m13, m14,
			m21, m23, m24,
			m31, m33, m34);
		Matrix3x3 M43(
			m11, m12, m14,
			m21, m22, m24,
			m31, m32, m34);
		Matrix3x3 M44(
			m11, m12, m13,
			m21, m22, m23,
			m31, m32, m33);

		Matrix4x4 m(
			M11.det(), -M12.det(), M13.det(), -M14.det(),
			-M21.det(), M22.det(), -M23.det(), M24.det(),
			M31.det(), -M32.det(), M33.det(), -M34.det(),
			-M41.det(), M42.det(), -M43.det(), M44.det());

		m = m.transpose();
		m = m / fabsf(this->det());

		return m;
	}

	float Matrix4x4::det()const 
	{
		// not tested!!!
		Matrix3x3 M11(
			m22, m23, m24,
			m32, m33, m34,
			m42, m43, m44);
		Matrix3x3 M12(
			m21, m23, m24,
			m31, m33, m34,
			m41, m43, m44);
		Matrix3x3 M13(
			m21, m22, m24,
			m31, m32, m34,
			m41, m42, m44);
		Matrix3x3 M14(
			m21, m22, m23,
			m31, m32, m33,
			m41, m42, m43);

		return m11*M11.det() - m12*M12.det() + m13*M13.det() - m14*M14.det();
	}

	Matrix3x3 Matrix4x4::toMatrix3x3()const 
	{
		return Matrix3x3(
			m11, m12, m13,
			m21, m22, m23,
			m31, m32, m33);
	}

	void clear()
	{
		m11 = m12 = m13 = m14 =
		m21 = m22 = m23 = m24 =
		m31 = m32 = m33 = m34 =
		m41 = m42 = m43 = m44 = 0.0f;
	}

	void identity()
	{
		clear();
		m11 = m22 = m33 = m44 = 1.0f;
	}

private:
	float det3x3(
		float m11, float m12, float m13,
		float m21, float m22, float m23,
		float m31, float m32, float m33)const
	{	}
};