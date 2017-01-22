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
		delete[] m_drawBuffer.pColorbuffer;
		delete[] m_drawBuffer.pDepthbuffer;
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
		std::vector<float> &vPos, int iPosChn,
		std::vector<float> &vNorm, int iNormChn,
		std::vector<float> &vTC, int iTCChn,
		std::vector<int> &vIdx);
	void setTexture(std::vector<float> &tex, int iTexChn, int w, int h, int idx);
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

	class Framebuffer
	{
	public:
		Color4f		*pColorbuffer;
		float		*pDepthbuffer;
		int			width;
		int			height;

		Framebuffer() :width(0), height(0)
		{	}
	};

	class Texture2D
	{
	public:
		std::vector<float>	texData;
		int					iChn;
		int					width;
		int					height;
	};

	typedef struct _GlobalConsts
	{
		/* Viewport */
		float	xk;
		float	yk;
		float	xb;
		float	yb;
		float	zk;
		float	zb;
	}GlobalConsts;

	/* Input */
	std::vector<float>	m_vPosition;
	std::vector<float>	m_vNormal;
	std::vector<float>	m_vTexcoord;
	std::vector<int>	m_vIndex;
	int					m_iPosChn;
	int					m_iNormChn;
	int					m_iTCChn;

	/* Statistics */
	int					m_iVertexCount;
	int					m_iPrimitiveCount;

	/* Internal data */
	Framebuffer			m_drawBuffer;

	/* States */
	Matrix4x4			m_mWorld;
	Matrix4x4			m_mView;
	Matrix4x4			m_mProj;
	Matrix4x4			m_mWVP;
	Matrix3x3			m_m33WV;
	Matrix3x3			m_m33World;
	Viewport			m_viewport;
	Texture2D			m_tex[16];

	float				m_zNear;

	/* Consts */
	GlobalConsts		m_consts;

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
		float &zn = m_zNear;
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
		Texture2D &tex = m_tex[i];
		if (tex.texData.size() == 0)
			return Color4f(1.0f, 0.0f, 1.0f, 1.0f);

		Vector2f frac(v.x - floor(v.x), v.y - floor(v.y));
		Vector2i pos((int)floor(frac.x*m_tex[i].width), (int)floor(frac.y*m_tex[i].height));
		int offset = (pos.x + pos.y*tex.height)*tex.iChn;
		// default RGBA
		Color4f color(tex.texData[offset], tex.texData[offset + 1], tex.texData[offset + 2], tex.texData[offset + 3]);
		return color;
	}

	inline float clamp(float f, float min, float max)
	{
		assert(min < max);
		return f > max ? max : (f < min ? min : f);
	}
};