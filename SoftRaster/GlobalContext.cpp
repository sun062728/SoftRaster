#include "GlobalContext.hpp"

#include "SceneManager.hpp"
#include "RenderEngine.hpp"

GlobalContext& GlobalContext::instance() {
	if (!gc) {
		gc = std::make_unique<GlobalContext>();
	}
	return *gc;
}

SceneManager& GlobalContext::sceneManagerInstance() {
	if (!sm_) {
		sm_ = std::make_unique<SceneManager>();
	}
	return *sm_;
}

RenderEngine& GlobalContext::renderEngineInstance() {
	if (!re_) {
		re_ = std::make_unique<RenderEngine>();
	}
	return *re_;
}


std::unique_ptr<GlobalContext> GlobalContext::gc;