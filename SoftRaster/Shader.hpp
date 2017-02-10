#pragma once

#include <memory>

#include "Utils\Vector.hpp"
#include "Raster\Raster.hpp"

class Shader {
public:
	virtual ~Shader() = default;
	virtual void VertexShader() = 0;
	virtual void InterpProps() = 0;
	virtual void PixelShader(void *pIn, void *pOut) = 0;
};

typedef std::shared_ptr<Shader> ShaderPtr;

class PhongShader :public Shader {
public:
	struct Constants {
		Vector3f lightDir = { 0.0f, -1.0f, -1.0f };
		Color4f lightColor = { 1.0f, 1.0f, 1.0f, 1.0f };
		Color4f diffuse = { 0.0f, 0.1f, 0.3f, 1.0f };
		float K = 0.3f;
		int specular = 20;
	};
	void VertexShader() override{

	}
	void PixelShader(void *pIn, void *pOut) override{

	}
	void InterpProps() override{
	}
};