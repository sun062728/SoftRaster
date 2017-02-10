#pragma once

#include "Prerequesite.hpp"
#include "Raster\Raster.hpp"

// prepare all data needed by raster
class RenderEngine {
public:
	void setVertexBuffer();
	void setIndexBuffer();
	void setTextures();
	void setShaders();
	void setShaderConstants();
	void render() const;
private:
	Raster raster_;
	ShaderPtr shader_;
};