#pragma once

#include "Demo.h"

struct GLFWwindow;

class BaseApp
{
public:
	virtual ~BaseApp() = default;

	virtual bool Run();

	void Update();

private:
	virtual bool Init() = 0;
	virtual bool RunInternal(GLFWwindow* window) = 0;

	double m_LastFrame = 0.0;

	Demo m_Demo;
};