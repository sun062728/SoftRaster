#include "Raster\Raster.hpp"
#include "Utils\ObjLoader.hpp"
#include "Utils\Matrix.hpp"
#include "Utils\Vector.hpp"
#include "Utils\Math.hpp"
#include "Utils\BMPLoader.hpp"

#include <sstream>
#include <fstream>
#include <string>
#include <istream>
#include <cassert>

#include "AppFramework.hpp"
#include "SceneManager.hpp"
#include "GlobalContext.hpp"
#include "Entity.hpp"
#include "Camera.hpp"
#include "Utils\Matrix.hpp"

int raw_style();

class CustomApp:public AppFramework {
public:
	void CustomApp::init() {
		auto sm = GlobalContext::instance().sceneManagerInstance();
		Matrix4x4 identity;
		identity.identity();
		auto entity = sm.CreateEntity("bunny", "./Resource/Mesh/bunny.obj", "TODO: matFileName", identity);
		auto ortho_cam = sm.CreateCamera("default ortho");
		sm.PushCamera(ortho_cam);
	}

	void CustomApp::run() {
		auto sm = GlobalContext::instance().sceneManagerInstance();
		sm.Update();
	}

	void CustomApp::destroy() {

	}
};

int main() {
	/*
	CustomApp app;
	app.init();
	app.run();
	app.destroy();
	*/

	raw_style();

	assert(!_CrtDumpMemoryLeaks());
}

int raw_style()
{
	///ObjLoader objLoader("./Resource/Mesh/triangle.obj");
	///ObjLoader objLoader("./Resource/Mesh/cube.obj");
	ObjLoader objLoader("./Resource/Mesh/bunny.obj");
	///ObjLoader objLoader("C:\\Users\\sun06\\Desktop\\Raster\\mesh\\Aventador\\Avent_tri.mtl.obj");
	bool isClockWise = false; // isClockWise: is left-hand coordinate system

	std::vector<float> vPos, vNorm, vTC;
	int iPosChn, iNormChn, iTCChn;
	std::vector<int> vIdx;
	constexpr int WIDTH = 1024;
	constexpr int HEIGHT = 1024;
	constexpr float RADIAN = 60.0f / 180.0f*(float)PI;
	Raster raster;
	/************ client code ************/
	// set transform
	// world
	Matrix4x4 mWorld = Math::matrixRotationY(0.0f); // (float)PI*0.25f
	if (!isClockWise) {
		Matrix4x4 mFlipZ;
		mFlipZ.identity();
		mFlipZ.m33 = -1.0f;
		mWorld = mFlipZ*mWorld;
	}
	raster.setWorldMatrix(mWorld);
	// view
	Vector3f eye(0.0f, 2.0f, -2.0f);
	Vector3f lookAt(0.0f, 1.0f, 0.0f);
	Vector3f up(0.0f, 1.0f, 0.0f);
	Matrix4x4 mView;
	Math::viewMatrix(eye, lookAt, up, mView);
	raster.setViewMatrix(mView);
	// projection
	Matrix4x4 mProj;
	///Math::perspectiveProjMatrix(RADIAN, (float)(WIDTH/HEIGHT), 1.0f, 1000.0f, mProj);
	Math::orthoProjMatrix(-3.0f, 3.0f, -3.0f, 3.0f, 0.0f, 5.0f, mProj);
	raster.setProjMatrix(mProj);
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
	// set textures
	std::vector<int> vTex;
	vTex.push_back(0); vTex.push_back(0); vTex.push_back(0);
	vTex.push_back(255); vTex.push_back(255); vTex.push_back(255);
	vTex.push_back(255); vTex.push_back(255); vTex.push_back(255);
	vTex.push_back(0); vTex.push_back(0); vTex.push_back(0);
	raster.setTexture(0, 2, 2, Texture2D::TF_RGBA32F, Texture2D::TF_RGB888, (void*)vTex.data());
	//BMPLoader texLoader("./Resource/Texture/Bitmap2Material_Pebbles.bmp");
	//raster.setTexture(0, texLoader.getWidth(), texLoader.getHeight(), Texture2D::TF_RGBA32F, Texture2D::TF_RGB888, texLoader.getData());
	// set vb, draw
	unsigned int maxObjNum = objLoader.getObjectNum();
	for (unsigned int iObj = 0; iObj < maxObjNum; iObj++) {
		unsigned int maxGrpNum = objLoader.getGroupNum(iObj);
		for (unsigned int iGrp = 0; iGrp < maxGrpNum; iGrp++) {
			std::string meshName;
			if (false == objLoader.getMeshName(iObj, iGrp, meshName))
				assert(0);
			if (false == objLoader.getMesh(0, 0, vPos, iPosChn, vNorm, iNormChn, vTC, iTCChn, vIdx))
				assert(0);

			raster.setVertexAttribs(vPos, 3, vNorm, 0, vTC, 0, vIdx);
			raster.draw();
		}
	}
	// dump raster result
	raster.dumpRT2BMP("./Output/Output.bmp");

	return 0;
}