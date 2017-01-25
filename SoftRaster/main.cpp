#include "Raster\Raster.hpp"
#include "Utils\ObjLoader.hpp"
#include "Utils\Matrix.hpp"
#include "Utils\Vector.hpp"
#include "Utils\Utils.hpp"


int main()
{
	///ObjLoader objLoader("C:\\Users\\sun06\\Desktop\\Raster\\triangle.obj");
	///ObjLoader objLoader("C:\\Users\\sun06\\Desktop\\Raster\\cube.obj");
	ObjLoader objLoader("C:\\Users\\sun06\\Desktop\\Raster\\bunny.obj");
	///ObjLoader objLoader("C:\\Users\\sun06\\Desktop\\Raster\\mesh\\Aventador\\Avent_tri.mtl.obj");
	std::vector<float> vPos, vNorm, vTC;
	int iPosChn, iNormChn, iTCChn;
	std::vector<int> vIdx;
	constexpr int WIDTH = 1024;
	constexpr int HEIGHT = 1024;
	constexpr float THETA = 60.0f;
	Raster raster;
	bool isClockWise = false;
	/************ client code ************/
	// set transform
	// world
	Matrix4x4 mWorld = Utils::matrixRotationY(0.0f); // (float)PI*0.25f
	if (!isClockWise) {
		Matrix4x4 trans;
		trans.identity();
		trans.m33 = -1.0f;
		mWorld = trans*mWorld;
	}
	raster.setModelMat(mWorld);
	// view
	Vector3f eye(0.0f, 2.0f, -2.0f);
	Vector3f lookAt(0.0f, 1.0f, 0.0f);
	Vector3f up(0.0f, 1.0f, 0.0f);
	Matrix4x4 mView;
	Utils::viewMatrix(eye, lookAt, up, mView);
	raster.setViewMat(mView);
	// projection
	Matrix4x4 mProj;
	///Utils::perspectiveProjMatrix(THETA, (float)(WIDTH/HEIGHT), 1.0f, 1000.0f, mProj);
	Utils::orthoProjMatrix(-3.0f, 3.0f, -3.0f, 3.0f, 0.0f, 5.0f, mProj);
	raster.setProjMat(mProj);
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
	raster.isClockWise(isClockWise);
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
	// dump raster result
	raster.dumpRT2BMP("C:\\Users\\sun06\\Desktop\\Raster\\Output.bmp");

	return 0;
}