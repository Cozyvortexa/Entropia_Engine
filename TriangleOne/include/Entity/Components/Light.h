#pragma once
#define NOMINMAX
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

#include "Camera.h"
#include "Modules/RenderModule/Shader.h"
#include "Entity/Components/Transform.h"

struct AABB {
	AABB() {};
	AABB(glm::vec3 newMin, glm::vec3 newMax)
	{
		min = newMin;
		max = newMax;
	}
public:
	glm::vec3 min;
	glm::vec3 max;
};


struct Light : Component{
public:
	Light() {};
	Light(glm::vec3 position, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float newIntensity);
	//Light(glm::vec3 position, glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float constant, float linear, float quadratique, float cutOff, float outerCutOff);

	//void UseLight(Shader* shader);
	//void UseShadow(Shader* shader);


	glm::vec3 position = glm::vec3(0);

	float intensity = 1.0f;
	glm::vec3 ambient = glm::vec3(0);
	glm::vec3 diffuse = glm::vec3(0);
	glm::vec3 specular = glm::vec3(0);

	unsigned int shadowWidth = 2048, shadowHeight = 2048;

protected:
	void InitBaseLight(glm::vec3 position, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float newIntensity);
	std::pair<unsigned int, unsigned int> InitShadowMap();
	std::pair<unsigned int, unsigned int> InitCubeMap();
};

struct DirLight : public Light {
	DirLight(glm::vec3 _position, glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular, glm::vec3 direction, std::shared_ptr<Shader> _depthShader, float newIntensity);
	glm::vec3 direction = glm::vec3(0);
	unsigned int depthMap = 0;
	unsigned int depthMapFBO = 0;

	//Shadow purpose
	float near_plane = 0.1f, far_plane = 100.0f;
	float orthoSize = 40.0f;

	glm::mat4 lightProjection = glm::mat4(1);
	glm::vec3 lightPos = glm::vec3(0);
	glm::mat4 lightViewMatrice = glm::mat4(1);
	glm::mat4 lightMatrice = glm::mat4(1);

	void UpdateMatrix(glm::mat4 projection,const glm::mat4 viewMatrice);



	std::shared_ptr<Shader> depthShader = nullptr;

private:
	glm::vec3 FrustumCenter(const std::vector<glm::vec3> corners);
	std::vector<glm::vec3> CalcWorldCorner(const glm::mat4 projection, glm::mat4 viewMatrice);
	AABB CalcBoundingBox(std::vector<glm::vec3> worldCorner);
	std::vector<glm::vec3> WorldCornerToLightSpace(glm::mat4 lightViewMatrice, std::vector<glm::vec3> worldCorners);

	std::vector<glm::vec3> ndcCubePoint;
};

struct PointLight : public Light{
public:
	PointLight(glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular, float _constant, float _linear, float _quadratique, std::shared_ptr<Shader> _depthShaderCubeMap);
	float constant = 1.0f;
	float linear = 0.0f;
	float quadratique = 0.0f;

	unsigned int depthCubeMapFBO;
	unsigned int depthCubeMap;

	//Shadow purpose
	float aspect = 0.0f;
	float near_plane = 0.1f;
	float far_plane = 50.0f; 

	std::shared_ptr<Shader> depthShaderCubeMap = nullptr;

};

class SpotLight : public Light {
public: 
	SpotLight(glm::vec3 _position, glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular, glm::vec3 _direction, float _constant, float _linear, float _quadratique, float _cutOff, float _outercutOff, float newIntensity);
	float constant;
	float linear;

	float quadratique;
	glm::vec3 direction = glm::vec3(0);
	float cutOff;
	float outerCutOff;
};