#pragma once
#include "Entity/Entity.h"
//#include "memory.h"


class Scene {
public:
	void RenderScene();
private:
	std::vector<std::shared_ptr<Entity>> listEntity;
};