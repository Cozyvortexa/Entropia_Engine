#pragma once
#include "Entity/Entity.h"
//#include "memory.h"
#include "Entity/Systemes/RenderSystem.h"
#include "Entity/Entity.h"

class Scene {
public:
	Scene(std::shared_ptr<RenderSystem> newRenderSystme);
	void RenderScene();

	template<typename EntityComponent> std::shared_ptr<EntityComponent> AddComponent(std::shared_ptr<Entity> currentEntity);

private:
	std::vector<std::shared_ptr<Entity>> listEntity;

	std::shared_ptr<RenderSystem> renderSystem;
};