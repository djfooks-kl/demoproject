#pragma once

struct GLFWwindow;

namespace flecs
{
    struct world;
}

namespace window_size_system
{
    void Update(flecs::world& world, GLFWwindow* window);
}
