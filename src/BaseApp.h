#pragma once

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
};