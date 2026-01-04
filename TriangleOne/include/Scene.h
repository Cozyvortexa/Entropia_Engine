#pragma once
#include "Entity/Entity.h"
//#include "memory.h"
#include "Entity/Systemes/RenderSystem.h"
#include "Entity/Entity.h"
#include <type_traits>

#include "memory.h"
#include <utility>

class Scene {
public:
	Scene(std::shared_ptr<RenderSystem> newRenderSystme);
	void RenderScene();


	template<typename T, typename... Args> std::shared_ptr<T> AddComponent(std::shared_ptr<Entity> currentEntity, Args&&... args) {
		std::shared_ptr<T> component = currentEntity->EntityCreateModules<T>(std::forward<Args>(args)...);

		if constexpr (std::is_same_v<T, MeshComponent>) {
			renderSystem->AddMeshComponent(component);
		}
		else if constexpr (std::is_same_v<T, DirLight>) {
			renderSystem->AddLightComponent(component);  // Prend en compte que les dir 
		}

		return component;
	}

	std::shared_ptr<Entity> CreateNewEntity();

private:
	std::vector<std::shared_ptr<Entity>> listEntity;

	std::shared_ptr<RenderSystem> renderSystem;
};