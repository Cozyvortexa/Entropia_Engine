#pragma once
#include <glm/gtc/type_ptr.hpp>

#include "ECS/System.h"
#include "ECS/World.h"
#include "ECS/Components/LightComponent.h"
#include "ECS/Components/PhysicComponent.h"

#include <tuple>
#include <iostream>
#include <vector>
#include <array>

#include "Render/Renderer.h"
#include "Render/RenderObject.h"
#include "Render/Binding_Points.h"

#define MAX_POINT_LIGHT 8
#define MAX_SPOT_LIGHT 8

#define MAX_SHADOW_CASTER_POINT_LIGHT 4
#define MAX_SHADOW_CASTER_SPOT_LIGHT 8


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
		void Draw_FinalPass(World* world, Resource::RenderResource* renderData, Resource::PhysicsResource* physicsData, glm::mat4 viewMatrice);
		void Draw_SkyBox(World* world, const Engine::Resource::ResourceBuffer* resourceBuffer, glm::mat4 viewMatrice);
		void LightningPass(World* world, Engine::Component::Transform* transformMainCamera, const Engine::Resource::ResourceBuffer* resourceBuffer, glm::mat4 viewMatrice);

#pragma endregion

#pragma region Init shadow 
		//SSBO
		void InitShadowSSBO(World& world, const Resource::ResourceBuffer* resourceBuffer);
		void InitShadowText_SSBO(World& world, const Resource::ResourceBuffer* resourceBuffer);

		void InitShadowArray(World& world, const Resource::ResourceBuffer* resourceBuffer);
		void InitShadowFBO(World& world, const Resource::ResourceBuffer* resourceBuffer);

		//Dir Light
		void InitDirLightFBO(World& world, const Resource::ResourceBuffer* resourceBuffer);
#pragma endregion

#pragma region Shadow

		void UpdateShadow(World* world, const Resource::ResourceBuffer* resourceBuffer);
		void ClearShadowsMap(World* world, const Resource::ResourceBuffer* resourceBuffer);
		void DrawShadow(World* world, const Resource::ResourceBuffer* resourceBuffer);
		void ShadowPass(World* world, const Resource::ResourceBuffer* resourceBuffer);

#pragma endregion

	private:
		void LightCollector(World* world, Engine::Component::CameraComponent* mainCamera, Engine::Resource::RenderResource* renderData);

		void UpdateLight(World* world, Engine::Resource::RenderResource* renderResource);

		glm::vec3 Calc_SpotLightDirection(glm::mat4 transformModel, glm::vec3 lightDirection);
	};
}