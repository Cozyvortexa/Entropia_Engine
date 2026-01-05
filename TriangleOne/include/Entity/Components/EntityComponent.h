#pragma once

#include <memory>
class Entity;
class Transform;


struct EntityComponent {
public:
	std::shared_ptr<Entity> entity;
	std::shared_ptr<Transform> transform; 
};