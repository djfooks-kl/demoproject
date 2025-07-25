#pragma once

#include <emscripten/html5.h>

#include "BaseApp.h"

class WebApp final : public BaseApp
{
public:
	WebApp();
	~WebApp() override;

	bool Init() override;
	bool RunInternal(GLFWwindow* window) override;

private:
	EMSCRIPTEN_WEBGL_CONTEXT_HANDLE m_WebGLContextHandle;
};