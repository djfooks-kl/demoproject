#pragma once

#include <unordered_map>

namespace demo
{
	struct InputComponent
	{
		std::unordered_map<int, bool> m_KeyDownMap;
	};
}