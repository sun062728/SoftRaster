#pragma once

#include "Prerequesite.hpp"
#include "AppFramework.hpp"

class CookTorranceApp :public AppFramework {
public:
	CookTorranceApp() {}
	virtual void init() override;
	virtual void run() override;
	virtual void destroy() override;
};