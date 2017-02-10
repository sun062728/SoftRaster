#pragma once

#include <array>

#include "Prerequesite.hpp"
#include "Utils\Vector.hpp"
#include "Raster\Texture.hpp"

class Material {
public:
	enum TextureSlot {
		TS_DIFFUSE,
		TS_AMBIENT,
		TS_SPECULAR,
		TS_EMISSIVE,
		TS_NORMAL,
		TS_HEIGHT,
		TS_NUMTEXSLOT,
	};
	typedef std::shared_ptr<std::array<Texture2D, TS_NUMTEXSLOT>> TextureArray;
	void setShader(ShaderPtr shader);
	void applyTextures() const;
	// send other data needed by raster
private:
	// shader
	ShaderPtr				shader_;
	// shader constants
	Color4f					diffuse_;
	Color4f					specular_;
	union {
		struct {
			float			m_glossness_;
			float			m_fresnelPower_;
			float			m_fresnelBias_;
			float			m_fresnelScale_;
		};
		float				channel2_[4];
	};
	// textures
	TextureArray			textures_;
	// raster states
	float					alphaTest_;
	bool					alphaBlend_;
};