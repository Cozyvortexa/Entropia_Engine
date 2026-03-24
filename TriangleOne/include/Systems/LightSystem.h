#pragma once
#include <glm/gtc/type_ptr.hpp>

#include "ECS/System.h"
#include "ECS/Components/Component.h"
#include "ECS/World.h"
#include "ECS/Components/Light.h"

#include <tuple>
#include <iostream>
#include <vector>

#include "Render/Renderer.h"

#define MAX_POINT_LIGHT 8
#define MAX_SPOT_LIGHT 8

#pragma region Padding_Structures
struct Padding_DirLight {  // To be identical to the memory alignment of structs in shaders, SSBO
	alignas(16) glm::vec3 direction;
	alignas(16) glm::vec3 ambient;
	alignas(16) glm::vec3 diffuse;
	alignas(16) glm::vec3 specular;
};
struct Padding_PointLight {
	alignas(16) glm::vec3 position;
	alignas(16) glm::vec3 ambient;
	alignas(16) glm::vec3 diffuse;
	alignas(16) glm::vec3 specular;
	float range = 0.0f;
};
struct Padding_SpotLight {
	alignas(16) glm::vec3 position;
	alignas(16) glm::vec3 direction;
	alignas(16) glm::vec3 ambient;
	alignas(16) glm::vec3 diffuse;
	alignas(16) glm::vec3 specular;
	float cutOff;
	float outerCutOff;
	float range;
	float padding;
};
struct All_Light {
	Padding_DirLight dirLight; // Only one DirLight
	std::vector<Padding_PointLight> pointLights;
	std::vector<Padding_SpotLight> spotLights;

	/////////// --- Shadow --- ///////////
	//Directional light
	unsigned int dirLight_DepthMap = 0;
	unsigned int dirLight_DepthMapFBO = 0;
	std::pair<unsigned int, unsigned int> dirLight_Shadow_Size;

	//Point lights
	std::vector<unsigned int> pointLights_DepthMap;
	std::vector<unsigned int> pointLights_DepthMapFBO;
	std::vector<std::pair<unsigned int, unsigned int>> pointLights_Shadow_Size;

	//Spot lights
	std::vector<unsigned int> spotLights_DepthMap;
	std::vector<unsigned int> spotLights_DepthMapFBO;
	std::vector<std::pair<unsigned int, unsigned int>> spotLights_Shadow_Size;

	/////////// --- Matrices --- /////////// 
	glm::mat4 dirLight_Matrice = glm::mat4(1.0f);
	std::vector <glm::mat4> spotLight_Matrice;
};
#pragma endregion


class LightSystem : public System {
	 void Init(World& world, const ResourceBuffer* resourceBuffer) override;
	 void Update(World& world, const ResourceBuffer* resourceBuffer) override;

	 void InitLightSSBO(World& world, const ResourceBuffer* renderResource);
#pragma region Init shadow buffer 
	 void InitShadowMap(DirLight* currentLight);
	 void InitCubeMap(PointLight* currentLight);
	 void InitSpotShadowMap(SpotLight* currentLight);
	 void InitShadowBuffer(World& world);

	 std::pair<unsigned int, unsigned int> CreateDummyShadowTextures();
#pragma endregion

#pragma region Draw Shadow
	 void DrawShadowForDirLight(World* world, RenderResource& renderResource, WindowResource& windowData, All_Light& currentLight);
	 void DrawShadowForPointLight(World* world, RenderResource& renderResource, WindowResource& windowData, All_Light& lights, int index);
	 void DrawShadowForSpotLight(World* world, RenderResource& renderResource, WindowResource& windowData, All_Light& lights, int index);
	 void ShadowPass(World* world, RenderResource* renderResource, WindowResource* windowResource, All_Light* lights);
#pragma endregion
private:
	All_Light* DataCollector(World* world, WindowResource* windowResource, CameraComponent* mainCamera);

	void UpdateLight(World* world, RenderResource* renderResource, All_Light& lights);

	void SendDepthMapToMainShader(World* world, const ResourceBuffer* resourceBuffer, All_Light* lights); // Temp

	glm::vec3 Calc_SpotLightDirection(glm::mat4 transformModel, glm::vec3 lightDirection);
};