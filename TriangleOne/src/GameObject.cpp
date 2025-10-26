#include "GameObject.h"


GameObject::GameObject(std::string model) {
	modelMesh = new Model(model);
}



void GameObject::DrawObject(Shader* shader, bool drawShadow) {

	glm::mat4 model = _model;
	
	model = glm::translate(model, _position);

	model = glm::rotate(model, glm::radians(_rotation.x), glm::vec3(1, 0, 0));
	model = glm::rotate(model, glm::radians(_rotation.y), glm::vec3(0, 1, 0));
	model = glm::rotate(model, glm::radians(_rotation.z), glm::vec3(0, 0, 1));

	model = glm::scale(model, glm::vec3(_scale));

	shader->setMatrix("model", model);

	if (drawShadow)
		modelMesh->DrawWithoutTexture(shader);
	else
		modelMesh->Draw(shader);
}
