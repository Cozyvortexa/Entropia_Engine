#pragma once
#include <glm/glm.hpp>

namespace Engine::Component {
	static const uint32_t INVALIDE_uint32_t = static_cast<uint32_t>(-1);
	struct Component {
		virtual ~Component() = default;
	};
}