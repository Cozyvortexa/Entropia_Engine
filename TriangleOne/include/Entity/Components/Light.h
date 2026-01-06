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
	unsigned int shadowWidth = 2048, shadowHeight = 2048;

protected:
	void InitBaseLight(glm::vec3 position, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular);
	void InitShadowMap(unsigned int depthMapFBO, unsigned int depthMap);
	void InitCubeMap(unsigned int depthCubeMapFBO, unsigned int depthCubemap);
};

struct DirLight : public Light {
	DirLight(glm::vec3 _position, glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular, glm::vec3 direction, std::shared_ptr<Shader> _depthShader);
	glm::vec3 direction;
	unsigned int depthMap;
	unsigned int depthMapFBO;

	//Shadow purpose
	float near_plane = 1.0f, far_plane = 50.0f;
	float orthoSize = 50.0f;
	float distance;

	glm::mat4 lightProjection;
	glm::vec3 lightPos;
	glm::mat4 lightView;

	glm::mat4 GetLightMatrice();


	std::shared_ptr<Shader> depthShader = nullptr;

	private:
		glm::mat4 lightMatrice;

};

struct PointLight : public Light{
public:
	PointLight(glm::vec3 _position, glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular, float _constant, float _linear, float _quadratique, std::shared_ptr<Shader> _depthShaderCubeMap);
	float constant;
	float linear;
	float quadratique;

	unsigned int depthCubeMapFBO;
	unsigned int depthCubeMap;

	//Shadow purpose
	float aspect;
	float near_plane;
	float far_plane; 

	std::shared_ptr<Shader> depthShaderCubeMap = nullptr;

};

class SpotLight : public Light {
public: 
	SpotLight(glm::vec3 _position, glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular, glm::vec3 _direction, float _constant, float _linear, float _quadratique, float _cutOff, float _outercutOff);
	float constant;
	float linear;

	float quadratique;
	glm::vec3 direction;
	float cutOff;
	float outerCutOff;
};