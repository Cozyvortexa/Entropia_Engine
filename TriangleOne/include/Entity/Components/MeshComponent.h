#pragma once
#include "Entity/Components/EntityComponent.h"
#include "Modules/RenderModule/ModelClass.h"
#include "memory.h"


struct MeshComponent : EntityComponent {
public:
	MeshComponent(std::string model, std::shared_ptr<Shader> _shader);

	bool haveToBeDraw = true;
	bool castShadow = true;

	std::shared_ptr<Shader> GetShader() { return shader; }
	std::shared_ptr<Model> modelMesh = nullptr;
private:

	std::shared_ptr<Shader> shader = nullptr;
};

