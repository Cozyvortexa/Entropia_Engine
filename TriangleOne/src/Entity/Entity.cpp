#include "Entity/Entity.h"

Entity::Entity() {
	transform = EntityCreateModules<Transform>();
	transform->transform = transform;  // Tkt c'est normal
	transform->entity = nullptr; 
}


//void GameObject::DrawObject(Shader* shader, bool drawShadow) {
//
//	glm::mat4 model = _model;
//	
//	model = glm::translate(model, _position);
//
//	model = glm::rotate(model, glm::radians(_rotation.x), glm::vec3(1, 0, 0));
//	model = glm::rotate(model, glm::radians(_rotation.y), glm::vec3(0, 1, 0));
//	model = glm::rotate(model, glm::radians(_rotation.z), glm::vec3(0, 0, 1));
//
//	model = glm::scale(model, glm::vec3(_scale));
//
//	shader->setMatrix("model", model);
//
//	if (drawShadow)
//		modelMesh->DrawWithoutTexture(shader);
//	else
//		modelMesh->Draw(shader);
//}



void Entity::EntityRemoveModules(std::shared_ptr<EntityComponent> module) {
	for (int i = 0; i < entityComponentList.size(); i++) {
		if (entityComponentList[i] == module) {
			//entityComponentList[i]->Shutdown();
			entityComponentList.erase(entityComponentList.begin() + i);
			return;
		}
		else {
			std::cout << "Component a retirer non trouver" << std::endl;
		}
	}
}
