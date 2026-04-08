#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ECS/System.h"
#include "ECS/Components/Component.h"
#include "ECS/World.h"

class CameraSystem : public System {
public:
	void Init(World& world, const ResourceBuffer* resourceBuffer) override;

	void Update(World& world, const ResourceBuffer* resourceBuffer) override;

};