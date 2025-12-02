#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

#include "Modules/RenderModule/Shader.h"
#include "Entity/Components/EntityComponent.h"

struct Light : EntityComponent{
public:
	Light() {};
	Light(glm::vec3 position, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular);
	//Light(glm::vec3 position, glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float constant, float linear, float quadratique, float cutOff, float outerCutOff);

	//void UseLight(Shader* shader);
	//void UseShadow(Shader* shader);


	glm::vec3 position;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	float intensity = 1.0f;
	const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;
};

struct DirLight : Light {
	DirLight(glm::vec3 _position, glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular, glm::vec3 direction);
	glm::vec3 direction;
};

struct PointLight : Light{
public:
	PointLight(glm::vec3 _position, glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular, float _constant, float _linear, float _quadratique);
	float constant;
	float linear;
	float quadratique;
};

class SpotLight : Light {
	SpotLight(glm::vec3 _position, glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular, float _constant, float _linear, float _quadratique);
	glm::vec3 direction;
	float cutOff;
	float outerCutOff;
};