#include "Utils.hpp"
#include <cmath>

namespace Utils
{
	Matrix4x4 matrixRotationX(float rad)
	{
		Matrix4x4 m;
		m.identity();
		m.m22 = cos(rad);
		m.m23 = sin(rad);
		m.m32 = -sin(rad);
		m.m33 = cos(rad);
		return m;
	}

	Matrix4x4 matrixRotationY(float rad) 
	{
		Matrix4x4 m;
		m.identity();
		m.m11 = cos(rad);
		m.m13 = -sin(rad);
		m.m31 = sin(rad);
		m.m33 = cos(rad);
		return m;
	}

	Matrix4x4 matrixRotationZ(float rad)
	{
		Matrix4x4 m;
		m.identity();
		m.m11 = cos(rad);
		m.m12 = sin(rad);
		m.m21 = -sin(rad);
		m.m22 = cos(rad);
		return m;
	}

	/*
	** Camera coordinate(Left-hand):
	**
	** +Z: lookAt - pos
	** +X: +Z x vUp
	** +Y: +X x +Z
	**
	** Vcam(result) * M = Vori:
	**
	** (1 0 0) * M = +X = (Xa Xb Xc) }            (Xa Xb Xc)
	** (0 1 0) * M = +Y = (Ya Yb Yc) } => E * M = (Ya Yb Yc)
	** (0 0 1) * M = +Z = (Za Zb Zc) }            (Za Zb Zc)
	**
	** 可以参考《3D游戏编程大师技巧》P427
	*/
	bool viewMatrix(
		const Vector3f &eye,
		const Vector3f &lookAt,
		const Vector3f &up,
		Matrix4x4 &mat)
	{
		Vector3f vz(lookAt - eye); vz = vz.normalize();
		Vector3f vx(vz.cross(up.normalize())); vx = vx.normalize();
		Vector3f vy(vx.cross(vz)); vy = vy.normalize();

		Matrix4x4 translate;
		translate.identity();
		translate.m41 = -eye.x; translate.m42 = -eye.y; translate.m43 = -eye.z;

		Matrix4x4 rot;
		rot.identity();
		rot.m11 = vx.x; rot.m12 = vx.y; rot.m13 = vx.z;
		rot.m21 = vy.x; rot.m22 = vy.y; rot.m23 = vy.z;
		rot.m31 = vz.x; rot.m32 = vz.y; rot.m33 = vz.z;

		mat = translate.Inv();
		mat = translate*rot.Inv();

		return true;
	}

	// Perspective projection matrix:
	// cot,		0.0f,				0.0f,					0.0f,
	// 0.0f,	cot/AspectRatio,	0.0f,					0.0f,
	// 0.0f,	0.0f,				(zf + zn) / (zf - zn),	1.0f,
	// 0.0f,	0.0f,				2 * zf*zn / (zn - zf),	0.0f,
	// cot = zn / 0.5w
	bool perspectiveProjMatrix(
		float thetaInDegree,	// in degree
		float aspectRatio,		// w/h
		float zn,
		float zf,
		Matrix4x4 &mat)
	{
		float cot = 1.0f / tan(thetaInDegree / 180.0f*(float)PI);
		mat.clear();
		mat.m11 = cot;
		mat.m22 = cot / aspectRatio;
		mat.m33 = (zf + zn) / (zf - zn);
		mat.m34 = 1.0f;
		mat.m43 = 2 * zf*zn / (zn - zf);

		return true;
	}

	bool orthoProjMatrix(
		float l, float r,
		float b, float t,
		float n, float f,
		Matrix4x4 &m)
	{
		m.clear();
		float rw = 1.0f / (r - l);
		float rh = 1.0f / (t - b);
		float rd = 1.0f / (f - n);
		m.m11 = 2.0f*rw;
		m.m41 = -2.0f*l*rw - 1.0f;
		m.m22 = 2.0f*rh;
		m.m42 = -2.0f*b*rh - 1.0f;
		m.m33 = rw;
		m.m43 = -1.0f*n*rd;
		m.m44 = 1.0f;

		return true;
	}
};