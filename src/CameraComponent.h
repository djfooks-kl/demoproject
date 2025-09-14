#pragma once

#include <glm/ext/matrix_float4x4.hpp>
#include <glm/vec2.hpp>

namespace demo
{
	struct CameraComponent
	{
		glm::vec2 m_Position = { 0.378f, 0.f };
		float m_Zoom = 2.27f;

		glm::mat4 m_View;
		glm::mat4 m_Projection;
		glm::mat4 m_ViewProjection;
	};
}