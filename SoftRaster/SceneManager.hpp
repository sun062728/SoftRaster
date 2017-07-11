#pragma once

#include <map>
#include <stack>
#include <vector>
#include <string>
#include <memory>

#include "Prerequesite.hpp"
#include "Light.hpp"
#include "Utils\Matrix.hpp"
#include "Utils\Vector.hpp"

class SceneManager {
public:
	SceneManager() {}
	~SceneManager();
	//copy ctor
	//copy assignment

	EntityPtr CreateEntity(std::string const &name,
		std::string const &meshFilename,
		std::string const &matFilename,
		Matrix4x4 worldMatrix);
	EntityPtr GetEntity(std::string const name);
	void RemoveEntity(EntityPtr target);
	void RemoveEntity(std::string const name);

	CameraPtr CreateCamera(std::string const name);
	CameraPtr GetCamera(std::string const name);
	inline void PushCamera(CameraPtr cam) {cameraStack_.push(cam);}
	inline void PopCamera() {cameraStack_.pop();}

	LightPtr AddLight() {}
	void RemoveLight(LightPtr target) {}
	void ClearLight() {}
	Color4f GetSkyLightColor() { return skyLightColor_; }

	std::vector<Light>	lights_;

	void Update();
private:
	void RenderVisible(CameraPtr cam);

	EntityLibrary				entityLib_;
	CameraLibrary				cameraLib_;
	std::stack<CameraPtr>		cameraStack_;
	Color4f						skyLightColor_;
};