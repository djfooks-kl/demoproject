#include "DemoSystem.h"

#include <cmath>
#include <flecs/flecs.h>

#include "DemoColorAnimationComponent.h"

namespace
{
    constexpr float s_Speed = 500.f;
}

void demo_system::Update(flecs::world& world, const double time, const float deltaTime)
{
    bool hasAny = false;
    auto query = world.query_builder<demo::ColorAnimationComponent>();
    query.each([&](demo::ColorAnimationComponent& color) {
        const uint8_t index = static_cast<uint8_t>(time) % 3;
        if (index == 0)
            color.m_R += static_cast<uint8_t>(deltaTime * s_Speed);
        else if (index == 1)
            color.m_G += static_cast<uint8_t>(deltaTime * s_Speed);
        else if (index == 2)
            color.m_B += static_cast<uint8_t>(deltaTime * s_Speed);

        hasAny = true;
    });

    if (!hasAny)
    {
        world.entity().add<demo::ColorAnimationComponent>();
    }
}
