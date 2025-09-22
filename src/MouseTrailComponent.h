#pragma once

#include <vector>
#include <glm/vec2.hpp>

namespace demo
{
	struct TrailPoint
	{
		glm::vec2 m_Position = { 0.f, 0.f };
		glm::vec3 m_Color = { 0.f, 0.f, 0.f };
	};

	struct MouseTrailComponent
	{
		std::vector<TrailPoint> m_Positions;
	};
}