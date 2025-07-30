#include "BaseApp.h"

#include <array>
#include <chrono>
#include <iostream>

#include <ImGui/backends/imgui_impl_glfw.h>
#include <ImGui/backends/imgui_impl_opengl3.h>
#include <ImGui/imgui.h>

#include "GLFWLib.h"

namespace
{
    void key_callback(GLFWwindow* window, int key, int /*scancode*/, int action, int /*mods*/)
    {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GLFW_TRUE);

        if (key == GLFW_KEY_W && action == GLFW_PRESS)
            std::cout << "yay!\n";
    }

    void error_callback(int /*error*/, const char* description)
    {
        std::cerr << "Error: " << description << "\n";
    }
}

void BaseApp::Update()
{
    const double time = glfwGetTime();
    const float deltaTime = static_cast<float>(time - m_LastFrame);
    m_LastFrame = time;

    glfwPollEvents();

    ImGui_ImplGlfw_NewFrame();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();

    m_Demo.Update(time, deltaTime);

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

bool BaseApp::Run()
{
    if (!glfwInit())
    {
        std::cout << "GLFW Initialization failed\n";
        return false;
    }

    glfwSetErrorCallback(error_callback);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    GLFWwindow* window = glfwCreateWindow(640, 480, "My Title", NULL, NULL);
    if (!window)
    {
        return false;
    }

    glfwMakeContextCurrent(window);

    if (!Init())
    {
        std::cout << "Initialization failed\n";
        return false;
    }

    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();
    ImGui::GetStyle().FontScaleDpi = 1.5f;

    glfwSetKeyCallback(window, key_callback);
    m_LastFrame = glfwGetTime();

    m_Demo.Init();

    RunInternal(window);

    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return true;
}