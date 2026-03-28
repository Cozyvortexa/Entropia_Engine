#pragma once

#include <glad/glad/glad.h>
#include <GLFW/glfw3.h>

#include <algorithm>

#include "Render/Texture.h"
#include "Render/Shader.h"
#include "Render/Material.h"

#include "ECS/Components/Component.h"
#include "ECS/AssetStore.h"

class Renderer {
public:
	Renderer(AssetStore* assetStore) { this->assetStore = assetStore; }

	void DrawMesh(Mesh& currentMesh);
	void DrawMesh_Without_Texture(Mesh& currentMesh);

private:
	AssetStore* assetStore;

};