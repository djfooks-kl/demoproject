#include "CameraInputSystem.h"

#include <cmath>
#include <flecs/flecs.h>

#include "CameraInputComponent.h"
#include "Core/GLFWLib.h"
#include "InputComponent.h"

namespace
{
    bool GetKeyValue(const demo::InputComponent& input, const int key)
    {
        auto itr = input.m_KeyDownMap.find(key);
        if (itr == input.m_KeyDownMap.end())
        {
            return false;
        }
        return itr->second;
    }
}

void camera_input_system::Update(flecs::world& world, double /*time*/, float /*deltaTime*/)
{
    auto query = world.query_builder<
        demo::CameraInputComponent,
        const demo::InputComponent>();
    query.each([&](
        demo::CameraInputComponent& cameraInput,
        const demo::InputComponent& input)
    {
        cameraInput.m_Position.x =
            (GetKeyValue(input, GLFW_KEY_A) ? -1.f : 0.f) +
             (GetKeyValue(input, GLFW_KEY_D) ? 1.f : 0.f);
        
        cameraInput.m_Position.y =
            (GetKeyValue(input, GLFW_KEY_S) ? -1.f : 0.f) +
             (GetKeyValue(input, GLFW_KEY_W) ? 1.f : 0.f);

        cameraInput.m_Zoom =
            (GetKeyValue(input, GLFW_KEY_E) ? -1.f : 0.f) +
             (GetKeyValue(input, GLFW_KEY_Q) ? 1.f : 0.f);
    });
}
