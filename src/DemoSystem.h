#pragma once

namespace flecs
{
    struct world;
}

namespace demo_system
{
    void Update(flecs::world& world, const double time, const float deltaTime);
}