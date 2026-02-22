#pragma once
#define NOMINMAX
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

#include "Camera.h"
#include "Systemes/RenderModule/Shader.h"
#include "ECS/Component.h"

struct AABB {
	AABB() { min = glm::vec3(0);  max = glm::vec3(0);};
	AABB(glm::vec3 newMin, glm::vec3 newMax)
	{
		min = newMin;
		max = newMax;
	}
public:
	glm::vec3 min;
	glm::vec3 max;
};


struct Light{
public:
	Light() {};
	Light(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float newIntensity);
	//Light(glm::vec3 position, glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float constant, float linear, float quadratique, float cutOff, float outerCutOff);

	//void UseLight(Shader* shader);
	//void UseShadow(Shader* shader);



	float intensity = 1.0f;
	glm::vec3 ambient = glm::vec3(0);
	glm::vec3 diffuse = glm::vec3(0);
	glm::vec3 specular = glm::vec3(0);

	unsigned int shadowWidth = 2048, shadowHeight = 2048;

protected:
	void InitBaseLight(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float newIntensity);
	std::pair<unsigned int, unsigned int> InitShadowMap();
	std::pair<unsigned int, unsigned int> InitCubeMap();
	std::pair<unsigned int, unsigned int> InitSpotShadowMap();
};

struct DirLight : public Light {
	DirLight(glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular, glm::vec3 direction, std::shared_ptr<Shader> _depthShader, float newIntensity);
	glm::vec3 direction = glm::vec3(0.0f,-1.0f,0.0f);
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
	PointLight(glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular, float _range, std::shared_ptr<Shader> _depthShaderCubeMap, float newIntesity);
	float range = 5.0f;

	unsigned int depthCubeMapFBO;
	unsigned int depthCubeMap;

	//Shadow purpose
	float aspect = 1.0f;
	float near_plane = 0.1f;

	std::shared_ptr<Shader> depthShaderCubeMap = nullptr;

};

struct SpotLight : public Light {
public: 
	SpotLight(glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular, glm::vec3 _direction, float _cutOff, float _outercutOff, float range, std::shared_ptr<Shader> depthShaderSpotMap, float newIntensity);
	float range = 1.0f;

	glm::vec3 direction = glm::vec3(0);
	float cutOff = 0.0f;
	float outerCutOff = 1.0f;

	float aspect = 1.0f;

	unsigned int depthMapFBO;
	unsigned int depthMap;

	glm::mat4 lightSpaceMatrix = glm::mat4(1.0f);  // Je sent le bug arriver avec celle la

	std::shared_ptr<Shader> depthSpotShaderMap = nullptr;
};