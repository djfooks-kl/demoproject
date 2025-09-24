#include "BaseApp.h"

#include <array>
#include <chrono>
#include <iostream>

#include <ImGui/backends/imgui_impl_glfw.h>
#include <ImGui/backends/imgui_impl_opengl3.h>
#include <ImGui/imgui.h>

#include "Core/GLFWLib.h"

namespace
{
    void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        BaseApp* app = static_cast<BaseApp*>(glfwGetWindowUserPointer(window));
        app->ProcessKeyInput(window, key, scancode, action, mods);
    }

    void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
    {
        BaseApp* app = static_cast<BaseApp*>(glfwGetWindowUserPointer(window));
        app->ProcessCursorInput(window, xpos, ypos);
    }

    void error_callback(int /*error*/, const char* description)
    {
        std::cout << "Error: " << description << "\n";
    }
}

void BaseApp::ProcessKeyInput(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);

    m_Demo.ProcessKeyInput(window, key, scancode, action, mods);
}

void BaseApp::ProcessCursorInput(GLFWwindow* window, double xpos, double ypos)
{
    m_Demo.ProcessCursorInput(window, xpos, ypos);
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

    m_Demo.Update(m_Window, time, deltaTime);

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

    m_Window = glfwCreateWindow(1080, 800, "Demo Project", NULL, NULL);
    if (!m_Window)
    {
        return false;
    }

    glfwMakeContextCurrent(m_Window);

    if (!Init())
    {
        std::cout << "Initialization failed\n";
        return false;
    }

    glfwSetWindowUserPointer(m_Window, this);
    glfwSetKeyCallback(m_Window, key_callback);
    glfwSetCursorPosCallback(m_Window, cursor_position_callback);

    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(m_Window, true);
    ImGui_ImplOpenGL3_Init();

    m_LastFrame = glfwGetTime();

    m_Demo.Init(m_Window);

    RunInternal(m_Window);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(m_Window);
    m_Window = nullptr;
    glfwTerminate();
    return true;
}