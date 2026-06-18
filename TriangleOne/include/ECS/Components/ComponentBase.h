#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Engine::Component {
	static const uint32_t INVALIDE_uint32_t = static_cast<uint32_t>(-1);
	struct Component {
		virtual ~Component() = default;
	};
}