#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

#include "Modules/RenderModule/Shader.h"


class Light {
public:
	enum Type {
		DirectionalLight,
		PointLight,
		SpotLight
	};
	Type lightType = DirectionalLight;
	Light(glm::vec3 position, glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular);
	Light(glm::vec3 position, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float constant, float linear, float quadratique);
	//Light(glm::vec3 position, glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float constant, float linear, float quadratique, float cutOff, float outerCutOff);

	void UseLight(Shader* shader);
	void UseShadow(Shader* shader);

	glm::vec3 GetPosition() { return _position; }
	void SetPosition(glm::vec3 newPos) { _position = newPos; }

private:

	glm::vec3 _position;
	glm::vec3 _direction;

	glm::vec3 _ambient;
	glm::vec3 _diffuse;
	glm::vec3 _specular;

	const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;

	//Point
	float _constant;
	float _linear;
	float _quadratique;

	//Spot
	float _cutOff;
	float _outerCutOff;
};