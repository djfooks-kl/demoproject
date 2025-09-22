#pragma once

#include <glm/vec2.hpp>
#include <unordered_map>

namespace demo
{
	struct InputComponent
	{
		std::unordered_map<int, bool> m_KeyDownMap;

		glm::vec2 m_WindowMouse = { 0.f, 0.f };
	};
}