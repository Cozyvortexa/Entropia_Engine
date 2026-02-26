#pragma once
#include "ECS/System.h"
#include "ECS/Component.h"

#include "ECS/World.h"
#include "Entity/Components/Light.h"

class LightSystem : System {
	 //void Init(World& world) override;
	 void Update(World& world, const ResourceBuffer* resourceBuffer) override;


	 //DrawShadowForDirLight


#pragma region Init shadow buffer 
	 //Shadow
	 void InitShadowBuffer(World& world);
	 void InitShadowMap(DirLight* currentLight);
	 void InitCubeMap(PointLight* currentLight);
	 void InitSpotShadowMap(SpotLight* currentLight);
#pragma endregion
};