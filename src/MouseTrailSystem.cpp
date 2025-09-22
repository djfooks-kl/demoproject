#include "MouseTrailSystem.h"

#include <flecs/flecs.h>

#include "CameraComponent.h"
#include "MouseTrailComponent.h"

namespace
{
    constexpr double s_Period = 2.0;
    constexpr float s_Saturation = 1.f;
    constexpr float s_Brightness = 1.f;
    constexpr int s_TrailSize = 500;

    glm::vec3 ConvertHSVToRgb(const float hue, const float saturation, const float brightness)
    {
        glm::vec3 rgb;
        const float c = brightness * saturation;
        const float x = c * (1 - std::abs(std::fmod(hue / 60.f, 2.f) - 1));
        const float m = brightness - c;

        if (0.f <= hue && hue < 60.f) {
            rgb = {c, x, 0.f};
        } else if (60.f <= hue && hue < 120.f) {
            rgb = {x, c, 0.f};
        } else if (120.f <= hue && hue < 180.f) {
            rgb = {0.f, c, x};
        } else if (180.f <= hue && hue < 240.f) {
            rgb = {0.f, x, c};
        } else if (240.f <= hue && hue < 300.f) {
            rgb = {x, 0.f, c};
        } else if (300.f <= hue && hue < 360.f) {
            rgb = {c, 0.f, x};
        } else {
            rgb = {0.f, 0.f, 0.f}; // Should not happen with hue in 0-360 range
        }

        rgb.r += m;
        rgb.g += m;
        rgb.b += m;

        return rgb;
    }
}

void mouse_trail_system::Update(flecs::world& world, const double time, const float /*deltaTime*/)
{
    auto query = world.query_builder<
        demo::MouseTrailComponent,
        const demo::CameraComponent>();
    query.each([&](
        demo::MouseTrailComponent& mouseTrail,
        const demo::CameraComponent& camera)
    {
        const double t = fmod(time, s_Period) / s_Period;
        const float hue = static_cast<float>(t) * 360.f;
        demo::TrailPoint p;
        p.m_Position = camera.m_WorldMouse;
        p.m_Color = ConvertHSVToRgb(hue, s_Saturation, s_Brightness);
        mouseTrail.m_Positions.push_back(p);
        if (mouseTrail.m_Positions.size() > s_TrailSize)
        {
            mouseTrail.m_Positions.erase(mouseTrail.m_Positions.begin());
        }
    });
}
