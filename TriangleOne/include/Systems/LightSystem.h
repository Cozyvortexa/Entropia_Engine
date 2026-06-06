#pragma once
#include <glm/gtc/type_ptr.hpp>

#include "ECS/System.h"
#include "ECS/World.h"
#include "ECS/Components/LightComponent.h"

#include <tuple>
#include <iostream>
#include <vector>

#include "Render/Renderer.h"

#define MAX_POINT_LIGHT 8
#define MAX_SPOT_LIGHT 8

#define LIGHT_BINDING_POINT 1
#define SHADOW_BINDING_POINT 2

#pragma region Padding_Structures
struct Padding_DirLight {  // To be identical to the memory alignment of structs in the shaders, SSBO
	alignas(16) glm::vec3 direction;
	alignas(16) glm::vec3 color;
};
struct Padding_PointLight {
	alignas(16) glm::vec3 position;
	alignas(16) glm::vec3 color;
	float range = 0.0f;
};
struct Padding_SpotLight {
	alignas(16) glm::vec3 position;
	alignas(16) glm::vec3 direction;
	alignas(16) glm::vec3 color;
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

namespace Engine::Systems {
	class LightSystem : public System {
		void Init(World& world, const Engine::Resource::ResourceBuffer* resourceBuffer) override;
		void Update(World& world, const Engine::Resource::ResourceBuffer* resourceBuffer) override;

#pragma region Init

		void InitLightSSBO(World& world, const Engine::Resource::ResourceBuffer* renderResource);
		void InitCaptureCubeMap(World& world, const Engine::Resource::ResourceBuffer* resourceBuffer);
		void InitPrefilter_IBL(World& world, Engine::Resource::RenderResource* renderData);
		void Init_IrradianceMap(World& world, const Engine::Resource::ResourceBuffer* resourceBuffer);
		void Init_BRDF_LUTTexture(World& world, const Engine::Resource::ResourceBuffer* resourceBuffer);
#pragma endregion

		void ConvulateEnvCube(World& world, const Engine::Resource::ResourceBuffer* resourceBuffer, glm::mat4 captureProjection, glm::mat4 captureViews[]);
		void Equirenctangular_To_CubeMap(World& world, const Engine::Resource::ResourceBuffer* resourceBuffer, std::string equirectangularMap_Path);
		void Prefilter_EnvCub(World& world, Engine::Resource::RenderResource* renderData, glm::mat4 captureProjection, glm::mat4 captureViews[]);

#pragma region Draw

		void Draw_BloomBlurEffect(Engine::Resource::RenderResource* renderData);
		void Draw_FinalPass(Engine::Resource::RenderResource* renderData);
		void Draw_SkyBox(World* world, const Engine::Resource::ResourceBuffer* resourceBuffer, glm::mat4 viewMatrice);
		void LightningPass(World* world, Engine::Component::Transform* transformMainCamera, const Engine::Resource::ResourceBuffer* resourceBuffer, glm::mat4 viewMatrice);

#pragma endregion

#pragma region Init shadow buffer 
		void InitShadowMap(Engine::Component::DirLight* currentLight);
		void InitCubeMap(Engine::Component::PointLight* currentLight);
		void InitSpotShadowMap(Engine::Component::SpotLight* currentLight);
		void InitShadowBuffer(World& world);

#pragma endregion

#pragma region Shadow
		void DrawShadowForDirLight(World* world, Engine::Resource::RenderResource& renderResource, All_Light& currentLight);
		void DrawShadowForPointLight(World* world, Engine::Resource::RenderResource& renderResource, All_Light& lights, int index);
		void DrawShadowForSpotLight(World* world, Engine::Resource::RenderResource& renderResource, All_Light& lights, int index);
		void ShadowPass(World* world, Engine::Resource::RenderResource* renderResource, All_Light* lights);
#pragma endregion

	private:
		All_Light* DataCollector(World* world, Engine::Component::CameraComponent* mainCamera, Engine::Resource::RenderResource* renderRessource);

		void UpdateLight(World* world, Engine::Resource::RenderResource* renderResource, All_Light& lights);

		void SendDepthMapToLightningShader(World* world, const Engine::Resource::RenderResource* renderResource, const Engine::Resource::ResourceBuffer* resourceBuffer, All_Light* lights);

		glm::vec3 Calc_SpotLightDirection(glm::mat4 transformModel, glm::vec3 lightDirection);
	};
}