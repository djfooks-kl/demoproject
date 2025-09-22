#include "InputSystem.h"

#include <flecs/flecs.h>

#include "InputComponent.h"
#include "GLFWLib.h"

void input_system::UpdateKeyInput(flecs::world& world, int key, int /*scancode*/, int action, int /*mods*/)
{
    world.query_builder<demo::InputComponent>()
        .each([&](demo::InputComponent& input)
    {
        if (action == GLFW_PRESS)
        {
            input.m_KeyDownMap[key] = true;
        }
        else if (action == GLFW_RELEASE)
        {
            input.m_KeyDownMap[key] = false;
        }
    });
}

void input_system::UpdateCursorInput(flecs::world& world, double xpos, double ypos)
{
    world.query_builder<demo::InputComponent>()
        .each([&](demo::InputComponent& input)
    {
        input.m_WindowMouse = { xpos, ypos };
    });
}