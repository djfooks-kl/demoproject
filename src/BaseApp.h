#pragma once

#include "Demo.h"

struct GLFWwindow;

class BaseApp
{
public:
    virtual ~BaseApp() = default;

    virtual bool Run();

    void Update();

    void ProcessKeyInput(GLFWwindow* window, int key, int scancode, int action, int mods);
    void ProcessCursorInput(GLFWwindow* window, double xpos, double ypos);

private:
    virtual bool Init() = 0;
    virtual bool RunInternal(GLFWwindow* window) = 0;

    double m_LastFrame = 0.0;

    Demo m_Demo;
    GLFWwindow* m_Window = nullptr;
};