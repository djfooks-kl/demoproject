#include "WindowSizeSystem.h"

#include <flecs/flecs.h>

#include "WindowSizeComponent.h"
#include "GLFWLib.h"

void window_size_system::Update(flecs::world& world, GLFWwindow* window)
{
    world.query_builder<demo::WindowSizeComponent>()
        .each([&](demo::WindowSizeComponent& windowSize)
    {
        glfwGetWindowSize(window, &windowSize.m_Width, &windowSize.m_Height);
    });
}