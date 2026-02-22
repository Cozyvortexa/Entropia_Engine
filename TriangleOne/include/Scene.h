#pragma once
#include "Entity/Entity.h"
#include <type_traits>

#include "memory.h"
#include <utility>

//class Scene {
//public:
//	Scene() = default;
//
//	Entity* CreateNewEntity();
//	const std::vector<std::unique_ptr<Entity>>& GetEntities() const { return listEntity; }
//private:
//	std::vector<std::unique_ptr<Entity>> listEntity;
//
//};