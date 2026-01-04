#include "Scene.h"

Scene::Scene(std::shared_ptr<RenderSystem> newRenderSystme) {
	renderSystem = newRenderSystme;
}

void Scene::RenderScene() {
	renderSystem->RenderMesh();
}

std::shared_ptr<Entity> Scene::CreateNewEntity() {
	std::shared_ptr<Entity> newEntity = std::make_shared<Entity>();
	listEntity.push_back(newEntity);
	return newEntity;
}

