#pragma once
#include "ECS/Component.h"

class World;

class System {
public:
	virtual void Init(World& world) {};
	virtual void Update(World& world, const ResourceBuffer* resourceBuffer) {};
	virtual void Shutdown(World& world) {};
};
