#pragma once

namespace flecs
{
    struct world;
}

namespace mouse_trail_system
{
    void Update(flecs::world& world, const double time, const float deltaTime);
}