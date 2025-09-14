#pragma once

namespace flecs
{
    struct world;
}

namespace input_system
{
    void UpdateInput(flecs::world& world, int key, int scancode, int action, int mods);
}