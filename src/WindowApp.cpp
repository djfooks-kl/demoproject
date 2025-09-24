#include "WindowApp.h"

#include <iostream>
#include <imgui.h>

#include "Core/GLFWLib.h"

WindowApp::~WindowApp()
{
}

WindowApp::WindowApp()
    : BaseApp()
{
}

bool WindowApp::Init()
{
    if (!gladLoadGLES2(glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return false;
    }

    glfwSwapInterval(1);

    return true;
}

bool WindowApp::RunInternal(GLFWwindow* window)
{
    printf("Running WindowApp...\n");

    ImGui::GetStyle().FontScaleDpi = 1.5f;

    while (!glfwWindowShouldClose(window))
    {
        Update();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    return true;
}
