#pragma once

#include "Utils\Vector.hpp"

struct Light {
	Color4f ambientColor;
	Color4f diffuseColor;
	Color4f specularColor;

	Vector3f worldPos;
	float radius;
	float attenuation;
};