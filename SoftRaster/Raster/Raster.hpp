#pragma once

#include <windows.h>
#include <vector>
#include <cassert>

#include "../Utils/Matrix.hpp"
#include "../Utils/Vector.hpp"

class Raster
{
public:
	Raster();
	~Raster()
	{
		delete[] drawBuffer_.pColorbuffer;
		delete[] drawBuffer_.pDepthbuffer;
	}
	void setModelMat(
		float _m11, float _m12, float _m13, float _m14,
		float _m21, float _m22, float _m23, float _m24,
		float _m31, float _m32, float _m33, float _m34,
		float _m41, float _m42, float _m43, float _m44);
	void setViewMat(
		float _m11, float _m12, float _m13, float _m14,
		float _m21, float _m22, float _m23, float _m24,
		float _m31, float _m32, float _m33, float _m34,
		float _m41, float _m42, float _m43, float _m44);
	void setProjMat(
		float _m11, float _m12, float _m13, float _m14,
		float _m21, float _m22, float _m23, float _m24,
		float _m31, float _m32, float _m33, float _m34,
		float _m41, float _m42, float _m43, float _m44);
	void setViewport(int x, int y, int w, int h);
	void setVertexAttribs(
		std::vector<float> const &vPos, int iPosChn,
		std::vector<float> const &vNorm, int iNormChn,
		std::vector<float> const &vTC, int iTCChn,
		std::vector<int> const &vIdx);
	void setTexture(std::vector<float> const &tex, int iTexChn, int w, int h, int idx);
	void clearColor(float r, float g, float b, float a);
	void clearDepthf(float d);
	void draw();
	void dumpRT2BMP(const char *path);

private:

	/* module interface */
	struct VtxProps
	{
		Vector4f	position;
		Vector3f	normal;
		Vector2f	texcoord;
		Vector4f	posWorld;
	};

	struct PixelProps
	{
		Vector2i	coord;
		Vector4f	posWorld;
		Vector3f	normal;
		Vector2f	texcoord;
	};

	struct IA2VS
	{
		VtxProps v1, v2, v3;
	};

	struct VS2TAS
	{
		VtxProps v1, v2, v3;
	};

	struct TASInternal
	{
		VtxProps v1, v2, v3;
	};

	struct TAS2RAS
	{
		VtxProps v1, v2, v3;
	};

	struct RAS2PS
	{
		PixelProps p;
	};

	struct PS2OM
	{

	};

	/*  */
	struct Viewport
	{
		int x, y, w, h;
		float zn, zf;
	};

	struct Framebuffer
	{
		Color4f		*pColorbuffer;
		float		*pDepthbuffer;
		int			width;
		int			height;

		Framebuffer() :width(0), height(0) {	}
	};

	struct Texture2D
	{
		std::vector<float>	texData;
		int					idx;
		int					width;
		int					height;
	};

	struct GlobalConsts
	{
		/* Viewport */
		float	xk,	yk, zk;
		float	xb,	yb, zb;
	};

	/* Input */
	std::vector<float>	vPosition_;
	std::vector<float>	vNormal_;
	std::vector<float>	vTexcoord_;
	std::vector<int>	vIndex_;
	int					iPosChn_;
	int					iNormChn_;
	int					iTCChn_;

	/* Statistics */
	int					iVertexCount_;
	int					iPrimitiveCount_;

	/* Internal data */
	Framebuffer			drawBuffer_;

	/* States */
	Matrix4x4			mWorld_;
	Matrix4x4			mView_;
	Matrix4x4			mProj_;
	Matrix4x4			mWVP_;
	Matrix3x3			m33WV_;
	Matrix3x3			m33World_;
	Viewport			viewport_;
	Texture2D			tex_[16];

	float				zNear_;

	/* Consts */
	GlobalConsts		consts_;

	/* Stages */
	void InputAssembler();
	void VertexShader(IA2VS ia2vs);
	void TriangleSetup_Clip(VS2TAS vs2tas);
	void TriangleSetup_PostClip(TASInternal tasInternal);
	void Rasterization(TAS2RAS tas2ras);
	void PixelShader(RAS2PS ras2ps);
	void PixelShader_fresnel(RAS2PS ras2ps);
	void PixelShader_cook_torrance(RAS2PS ras2ps);
	void OutputMerger(PS2OM ps2om);

	/* Helpers */
	void init();
	void initStatesAndInternalConstants();
	inline void doZNearClip(const VtxProps &v1, const VtxProps &v2, VtxProps &vC)
	{
		float &zn = zNear_;
		float alpha = (zn + v1.position.z) / (v1.position.z - v2.position.z);
		float oneMinusAlpha = 1.0f - alpha;

		vC.position.x = oneMinusAlpha*v1.position.x + alpha*v2.position.x;
		vC.position.y = oneMinusAlpha*v1.position.y + alpha*v2.position.y;
		vC.position.z = oneMinusAlpha*v1.position.z + alpha*v2.position.z;
		vC.position.w = oneMinusAlpha*v1.position.w + alpha*v2.position.w; // CHECK IF w == zNear!!!
		///assert(vC.position.w == m_zNear);
		vC.normal.x = oneMinusAlpha*v1.normal.x + alpha*v2.normal.x;
		vC.normal.y = oneMinusAlpha*v1.normal.y + alpha*v2.normal.y;
		vC.normal.z = oneMinusAlpha*v1.normal.z + alpha*v2.normal.z;
		vC.texcoord.x = oneMinusAlpha*v1.texcoord.x + alpha*v2.texcoord.x;
		vC.texcoord.y = oneMinusAlpha*v1.texcoord.y + alpha*v2.texcoord.y;
		vC.posWorld.x = oneMinusAlpha*v1.posWorld.x + alpha*v2.posWorld.x;
		vC.posWorld.y = oneMinusAlpha*v1.posWorld.y + alpha*v2.posWorld.y;
		vC.posWorld.z = oneMinusAlpha*v1.posWorld.z + alpha*v2.posWorld.z;
		vC.posWorld.w = 1.0f;
	}

	// nearest sampler
	inline Color4f tex2D(const Vector2f &v, int i/* Tex idx*/) 
	{
		Texture2D &tex = tex_[i];
		if (tex.texData.size() == 0)
			return Color4f(1.0f, 0.0f, 1.0f, 1.0f);

		Vector2f frac(v.x - floor(v.x), v.y - floor(v.y));
		Vector2i pos((int)floor(frac.x*tex_[i].width), (int)floor(frac.y*tex_[i].height));
		int offset = (pos.x + pos.y*tex.height)*tex.idx;
		// default RGBA
		Color4f color(tex.texData[offset], tex.texData[offset + 1], tex.texData[offset + 2], tex.texData[offset + 3]);
		return color;
	}

	inline float clamp(float f, float min, float max) const
	{
		assert(min < max);
		return f > max ? max : (f < min ? min : f);
	}
};