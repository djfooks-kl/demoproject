#pragma once

namespace flecs
{
    struct world;
}

namespace camera_input_system
{
    void Update(flecs::world& world, double time, float deltaTime);
}