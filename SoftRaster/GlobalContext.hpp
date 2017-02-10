#pragma once

#include <memory>

#include "Prerequesite.hpp"

class GlobalContext {
public:
	static GlobalContext& GlobalContext::instance();
	SceneManager& sceneManagerInstance();
	RenderEngine& renderEngineInstance();
private:
	static std::unique_ptr<GlobalContext> gc;
	std::unique_ptr<SceneManager> sm_;
	std::unique_ptr<RenderEngine> re_;
};