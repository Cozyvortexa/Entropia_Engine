#pragma once
#include "Entity/Components/EntityComponent.h"
#include "Entity/Components/Light.h"
#include "Entity/Components/MeshComponent.h"

#include <vector>

class RenderSystem {
public:
	void RenderMesh();

private:
	void UpdateLight(std::shared_ptr<Shader> shader);
	std::vector<std::shared_ptr<MeshComponent>> modeleList;

	std::vector<std::shared_ptr<DirLight>> directionalLightList;
	std::vector<std::shared_ptr<PointLight>> pointLightList;
	std::vector<std::shared_ptr<SpotLight>> spotLightList; 
};
