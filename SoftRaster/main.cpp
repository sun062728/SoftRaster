#include <iostream>

#include "Raster\Raster.hpp"
#include "Utils\ObjLoader.hpp"
#include "Utils\Matrix.hpp"
#include "Utils\Vector.hpp"
#include "Utils\Utils.hpp"


int main()
{
	//ObjLoader objLoader("C:\\Users\\sun06\\Desktop\\Raster\\cube.obj");
	ObjLoader objLoader("C:\\Users\\sun06\\Desktop\\Raster\\bunny.obj");
	//ObjLoader objLoader("C:\\Users\\sun06\\Desktop\\Raster\\mesh\\Aventador\\Avent_tri.mtl.obj");
	std::vector<float> vPos, vNorm, vTC;
	int iPosChn, iNormChn, iTCChn;
	std::vector<int> vIdx;
	constexpr int WIDTH = 1024;
	constexpr int HEIGHT = 1024;
	constexpr float THETA = 60.0f;

	Raster raster;
	// set transform
	Matrix4x4 mWorld;
	mWorld.identity();
	raster.setModelMat(
		mWorld.m11, mWorld.m12, mWorld.m13, mWorld.m14,
		mWorld.m21, mWorld.m22, mWorld.m23, mWorld.m24,
		mWorld.m31, mWorld.m32, mWorld.m33, mWorld.m34,
		mWorld.m41, mWorld.m42, mWorld.m43, mWorld.m44);

	Vector3f eye(0.0f, 2.0f, -2.0f);
	Vector3f lookAt(0.0f, 1.0f, 0.0f);
	Vector3f up(0.0f, 1.0f, 0.0f);
	Matrix4x4 mView;

	Utils::viewMatrix(eye, lookAt, up, mView);
	raster.setViewMat(
		mView.m11, mView.m12, mView.m13, mView.m14,
		mView.m21, mView.m22, mView.m23, mView.m24,
		mView.m31, mView.m32, mView.m33, mView.m34,
		mView.m41, mView.m42, mView.m43, mView.m44);

	Matrix4x4 mProj;
	//Utils::perspectiveProjMatrix(THETA, (float)(WIDTH/HEIGHT), 1.0f, 1000.0f, mProj);
	Utils::orthoProjMatrix(-3.0f, 3.0f, -3.0f, 3.0f, 0.0f, 5.0f, mProj);
	raster.setProjMat(
		mProj.m11, mProj.m12, mProj.m13, mProj.m14,
		mProj.m21, mProj.m22, mProj.m23, mProj.m24,
		mProj.m31, mProj.m32, mProj.m33, mProj.m34,
		mProj.m41, mProj.m42, mProj.m43, mProj.m44);
	raster.setViewport(0, 0, WIDTH, HEIGHT);
	// set color & depth buffer
	raster.clearColor(0.1f, 0.1f, 0.1f, 1.0f);
	raster.clearDepthf(1.0f);
	// set ps lighting
	raster.setPSBaseColor(Color4f(0.0f, 0.1f, 0.3f, 1.0f));
	raster.setPSDiffuseColor(Color4f(0.88f, 0.88f, 0.88f, 1.0f));
	raster.setPSLightColor(Color4f(1.0f, 1.0f, 1.0f, 1.0f));
	raster.setPSEyePos(eye);
	raster.setPSLightDir(Vector3f(0.0f, -1.0f, -0.0f));
	raster.setPSSpecularPower(25);
	// set winding rule
	raster.isLeftHand(false);
	// set vb, draw
	unsigned int maxObjNum = objLoader.getObjectNum();
	for (unsigned int iObj = 0; iObj < maxObjNum; iObj++) {
		unsigned int maxGrpNum = objLoader.getGroupNum(iObj);
		for (unsigned int iGrp = 0; iGrp < maxGrpNum; iGrp++) {
			char szMeshName[1024];
			if (false == objLoader.getMeshName(iObj, iGrp, szMeshName, 1024))
				assert(0);
			if (false == objLoader.getMesh(0, 0, vPos, iPosChn, vNorm, iNormChn, vTC, iTCChn, vIdx))
				assert(0);

			if (iPosChn != 3) assert(0);
			if (!(iNormChn != 3 || iNormChn != 0)) assert(0);
			if (!(iTCChn != 2 || iTCChn != 0)) assert(0);

			raster.setVertexAttribs(vPos, 3, vNorm, 0, vTC, 0, vIdx);

			//std::vector<float> vTex;
			//vTex.push_back(0.0f); vTex.push_back(0.0f); vTex.push_back(0.0f); vTex.push_back(1.0f);
			//vTex.push_back(1.0f); vTex.push_back(1.0f); vTex.push_back(1.0f); vTex.push_back(1.0f);
			//vTex.push_back(1.0f); vTex.push_back(1.0f); vTex.push_back(1.0f); vTex.push_back(1.0f);
			//vTex.push_back(0.0f); vTex.push_back(0.0f); vTex.push_back(0.0f); vTex.push_back(1.0f);

			//raster.setTexture(vTex, 4, 2, 2, 0);
			char str[64];
			sprintf_s(str, 64, "Obj %d, Grp %d\n", iObj, iGrp);
			OutputDebugString(str);
			raster.draw();
		}
	}

	raster.dumpRT2BMP("C:\\Users\\sun06\\Desktop\\Raster\\Output.bmp");

	return 0;
}