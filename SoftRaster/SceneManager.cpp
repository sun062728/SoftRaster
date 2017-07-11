#include <cassert>

#include "SceneManager.hpp"
#include "Entity.hpp"
#include "Camera.hpp"
#include "OrthogonalCamera.hpp"
#include "PerspectiveCamera.hpp"
#include "Utils\Vector.hpp"

SceneManager::~SceneManager() {
	for (auto pair : cameraLib_) {
		pair.second.reset();
	}
	for (auto pair : entityLib_) {
		pair.second.reset();
	}
}

EntityPtr SceneManager::CreateEntity(std::string const &name,
	std::string const &meshFilename,
	std::string const &matFilename,
	Matrix4x4 worldMatrix) {
	auto it = entityLib_.find(name);
	if (it != entityLib_.end()) {
		return it->second;
	}

	EntityPtr e = std::make_shared<Entity>(meshFilename);
	e->setWorldMatrix(worldMatrix);

	entityLib_.insert({ name, e });
	return e;
}

EntityPtr SceneManager::GetEntity(std::string const name) {
	auto it = entityLib_.find(name);
	if (it != entityLib_.cend()) {
		return it->second;
	}
	return EntityPtr();
}

void SceneManager::RemoveEntity(EntityPtr target) {
	for (auto it = entityLib_.begin(); it != entityLib_.end(); it++) {
		if (it->second == target) {
			entityLib_.erase(it);
		}
	}
}

void SceneManager::RemoveEntity(std::string const name) {
	auto it = entityLib_.find(name);
	if (it != entityLib_.cend()) {
		entityLib_.erase(it);
	}
}

CameraPtr SceneManager::CreateCamera(std::string const name) {
	auto it = cameraLib_.find(name);
	if (it != cameraLib_.cend()) {
		return it->second;
	}
	auto cam = std::make_shared<OrthogonalCamera>();
	cam->orthoParams(-3.0f, 3.0f, -3.0f, 3.0f, 0.0f, 5.0f);
	cameraLib_.insert({ name, cam });
	return cam;
}

CameraPtr SceneManager::GetCamera(std::string const name) {
	auto it = cameraLib_.find(name);
	if (it != cameraLib_.cend()) {
		return it->second;
	}
	return CameraPtr();
}

void SceneManager::Update() {
	if (cameraStack_.empty()) {
		assert(0);
		return;
	}
	auto cam = cameraStack_.top();
	for (auto pair : entityLib_) {
		if (auto entityPtr = pair.second) {
			//entityPtr->update();
		}
	}
	RenderVisible(cam);
}

void SceneManager::RenderVisible(CameraPtr cam) {
	// do cull

	// cam->markDirty();

	for (auto pair : entityLib_) {
		if (pair.second) {
			// set world, view, proj matrices
			pair.second->draw();
		}
	}
}











