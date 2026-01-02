#include "Scene.h"

Scene::Scene(std::shared_ptr<RenderSystem> newRenderSystme) {
	renderSystem = newRenderSystme;
}

void Scene::RenderScene() {
	renderSystem->RenderMesh();
}

template<typename EntityComponent> std::shared_ptr<EntityComponent> Scene::AddComponent(std::shared_ptr<Entity> currentEntity) {
	std::shared_ptr<EntityComponent> component = currentEntity->EntityCreateModules<EntityComponent>();

	switch (EntityComponent)
	{
		case MeshComponent :
			renderSystem->AddMeshComponent(component);
			break;
		default:
			break;
	}
	return module;
}