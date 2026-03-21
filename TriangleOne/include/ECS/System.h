#pragma once
#include "ECS/Components/Component.h"

class World;

class System {
public:
	virtual void Init(World& world, const ResourceBuffer* resourceBuffer) {};
	virtual void Update(World& world, const ResourceBuffer* resourceBuffer) {};
	virtual void Shutdown(World& world) {};
};
