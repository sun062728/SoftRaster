#include "CookTorranceApp.hpp"
#include "SceneManager.hpp"
#include "GlobalContext.hpp"
#include "Entity.hpp"
#include "Camera.hpp"
#include "Utils\Matrix.hpp"

void CookTorranceApp::init() {
	auto sm = GlobalContext::instance().sceneManagerInstance();
	Matrix4x4 identity;
	identity.identity();
	auto entity = sm.CreateEntity("bunny", "./Resource/Mesh/bunny.obj", " ", identity);
	auto ortho_cam = sm.CreateCamera("default ortho");
	sm.PushCamera(ortho_cam);
}

void CookTorranceApp::run() {
	auto sm = GlobalContext::instance().sceneManagerInstance();
	sm.Update();
}

void CookTorranceApp::destroy() {

}
