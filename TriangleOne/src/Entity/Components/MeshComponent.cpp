#include "Entity/Components/MeshComponent.h"


MeshComponent::MeshComponent(std::string model, std::shared_ptr<Shader> _shader) {
	modelMesh = std::make_shared<Model>(model);
	shader = _shader;
}
