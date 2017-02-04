#pragma once


#define WIN32_LEAN_AND_MEAN
#include <windows.h>	// just for bmp header

#include <vector>
#include <cassert>

#include "../Utils/Matrix.hpp"
#include "../Utils/Vector.hpp"

#include "Texture.hpp"

/*
** Left-hand coordinate system
** Clock-wise winding is default
*/
class Raster
{
public:
	Raster();
	~Raster()
	{
		delete[] drawBuffer_.pColorbuffer;
		delete[] drawBuffer_.pDepthbuffer;
	}
	void setWorldMatrix(Matrix4x4 m);
	void setViewMatrix(Matrix4x4 m);
	void setProjMatrix(Matrix4x4 m);
	void setViewport(int x, int y, int w, int h);
	void setVertexAttribs(
		std::vector<float> const &vPos, int iPosChn,
		std::vector<float> const &vNorm, int iNormChn,
		std::vector<float> const &vTC, int iTCChn,
		std::vector<int> const &vIdx);
	void setTexture(int idx, unsigned int w, unsigned int h, Texture2D::TexFmt internalFormat, Texture2D::TexFmt dataFormat, void *pData);
	void clearColor(float r, float g, float b, float a);
	void clearDepthf(float d);
	void draw();
	void dumpRT2BMP(const char *path);

	void setPSEyePos(Vector3f v);
	void setPSLightDir(Vector3f v);
	void setPSBaseColor(Color4f c);
	void setPSDiffuseColor(Color4f c);
	void setPSLightColor(Color4f c);
	void setPSSpecularPower(int f);

	void isClockWise(bool b);

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

	struct GlobalConsts
	{
		/* Viewport */
		float	xk,	yk, zk;
		float	xb,	yb, zb;
	};

	struct PSLighting {
		Vector3f	eyePos;
		Vector3f	lightDir;
		Color4f		baseColor;
		Color4f		diffuseColor;
		Color4f		lightColor;
		int			specularPower;
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
	unsigned int		iVertexCount_;
	unsigned int		iPrimitiveCount_;

	/* Internal data */
	Framebuffer			drawBuffer_;
	Texture2D			textures_[8];

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
	bool				bClockWise_;

	/* Consts */
	GlobalConsts		consts_;

	/* PS lighting property */
	PSLighting			psLighting_;

	/* Stages */
	void InputAssembler();
	void VertexShader(IA2VS ia2vs);
	void TriangleSetup_Clip(VS2TAS vs2tas);
	void TriangleSetup_PostClip(TASInternal tasInternal);
	void Rasterization_float(TAS2RAS tas2ras);
	void Rasterization_integer(TAS2RAS tas2ras);
	void PixelShader(RAS2PS ras2ps);
	void PixelShader_phong(RAS2PS ras2ps);
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

	inline float clamp(float f, float min, float max) const
	{
		assert(min < max);
		return f > max ? max : (f < min ? min : f);
	}
};