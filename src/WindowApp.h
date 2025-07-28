#pragma once

#include "BaseApp.h"

class WindowApp final : public BaseApp
{
public:
    WindowApp();
    ~WindowApp() override;

private:
    bool Init() override;
    bool RunInternal(GLFWwindow* window) override;
};