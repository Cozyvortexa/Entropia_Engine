#pragma once
#include "ECS/System.h"
#include "ECS/Component.h"

#include "ECS/World.h"
#include "Entity/Components/Light.h"

#include <tuple>

class LightSystem : System {
	 //void Init(World& world) override;
	 void Update(World& world, const ResourceBuffer* resourceBuffer) override;



#pragma region Init shadow buffer 
	 void InitShadowMap(DirLight* currentLight);
	 void InitCubeMap(PointLight* currentLight);
	 void InitSpotShadowMap(SpotLight* currentLight);
	 void InitShadowBuffer(World& world);
#pragma endregion

#pragma region Draw Shadow
	 void DrawShadowForDirLight(World* world, WindowResource* windowData, DirLight* currentLight);
	 void DrawShadowForPointLight(World* world, WindowResource* windowData, PointLight* currentLight, Transform* transform);
	 void DrawShadowForSpotLight(World* world, WindowResource* windowData, SpotLight* currentLight, Transform* transform);
	 void UpdateShadow(World* world, const ResourceBuffer* resourceBuffer);
#pragma endregion
private:
	void SendDepthMapToMainShader(World* world, const ResourceBuffer* resourceBuffer,
		std::vector<unsigned int> stars_DepthMap, 
		std::vector<unsigned int> pointLights_DepthMap, 
		std::vector<unsigned int> spotLights_DepthMap); // Temp
};