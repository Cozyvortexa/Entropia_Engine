#pragma once
#include "Modules/RenderModule/Shader.h"

#include "Modules/RenderModule/ModelClass.h"
#include "Entity/Components/EntityComponent.h"
#include "Entity/Components/Transform.h"

#include <utility>
#include <memory>
#include <string>
#include <vector>

class Entity : public std::enable_shared_from_this<Entity> {
public:
	Entity();
	
	template<typename T, typename... Args> std::shared_ptr<T> EntityCreateModules(Args&&... args) {
		auto module = std::make_shared<T>(std::forward<Args>(args)...);
		entityComponentList.push_back(module);


		//module->entity = this;
		transform;
		module->transform = transform;
		module->entity = shared_from_this();
		return module;
	}

	void EntityRemoveModules(std::shared_ptr<EntityComponent> module);

	template<typename T> std::shared_ptr<T> GetComponent(std::shared_ptr<Entity> currentEntity) {
		for (std::shared_ptr<EntityComponent> currentComponent : currentEntity->entityComponentList) {
			if constexpr (std::is_same_v<T, currentComponent.get()>) {
				return currentComponent;
			}
		}
		return nullptr;
	}


	std::string name;
	std::shared_ptr<Transform> transform;

private: 
	std::vector<std::shared_ptr<EntityComponent>> entityComponentList;
	int id = 0;  // Go voir apres

	//Pour plus tard
	//std::vector<std::shared_ptr<Entity>> childrenEntity;
};
