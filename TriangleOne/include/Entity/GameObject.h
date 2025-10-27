#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Modules/RenderModule/Shader.h"

#include "Modules/RenderModule/ModelClass.h"

class GameObject {
public:
	GameObject(std::string model);

	void SetPosition(glm::vec3 newPos) { _position = newPos; }
	void SetRotation(glm::vec3 newRotation) { _rotation = newRotation; }
	void SetScale(glm::vec3 newScale) { _scale = newScale; }

	glm::vec3 GetPosition() { return _position; }

	void DrawObject(Shader* shader, bool drawShadow);


private: 
	Model* modelMesh;
	glm::mat4 _model = glm::mat4(1.0f);

	glm::vec3 _position = glm::vec3(0.0f);
	glm::vec3 _rotation = glm::vec3(1.0f);
	glm::vec3 _scale = glm::vec3(1.0f);

	glm::mat4 currentModel;
};
