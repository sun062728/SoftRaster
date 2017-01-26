#pragma once

#include <vector>
#include "Matrix.hpp"
#include "Vector.hpp"

namespace Math
{
	Matrix4x4 matrixRotationX(float rad);
	Matrix4x4 matrixRotationY(float rad);
	Matrix4x4 matrixRotationZ(float rad);

	bool viewMatrix(
		const Vector3f &eye,
		const Vector3f &lookAt,
		const Vector3f &up,
		Matrix4x4 &mat);

	bool perspectiveProjMatrix(
		float thetaInDegree,	// in degree
		float aspectRatio,		// w/h
		float zn,
		float zf,
		Matrix4x4 &mat);

	bool orthoProjMatrix(
		float l, float r,
		float b, float t,
		float n, float f,
		Matrix4x4 &m);

	inline int iRound(float f)
	{
		return f >= 0.0f ? static_cast<int>(f + 0.5f) : static_cast<int>(f - 0.5f);
	}
};