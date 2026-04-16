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
	Renderer(AssetStore* assetStore) { 
		this->assetStore = assetStore; 
		LoadDefaultCube(); 
	}

	void DrawMesh(Mesh& currentMesh);
	void DrawMesh_Without_Texture(Mesh& currentMesh);

	void DrawQuad(RenderResource* renderData);

	void DrawCube();

private:
	AssetStore* assetStore;
	void LoadDefaultCube();


	unsigned int cubeVAO = -1;
	unsigned int cubeVBO = -1;
    float cubeVertices[108] = {
        // back face
        -1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        // front face
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,
        // left face
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        // right face
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
         // bottom face
         -1.0f, -1.0f, -1.0f,
          1.0f, -1.0f, -1.0f,
          1.0f, -1.0f,  1.0f,
          1.0f, -1.0f,  1.0f,
         -1.0f, -1.0f,  1.0f,
         -1.0f, -1.0f, -1.0f,
         // top face
         -1.0f,  1.0f, -1.0f,
          1.0f,  1.0f , 1.0f,
          1.0f,  1.0f, -1.0f,
          1.0f,  1.0f,  1.0f,
         -1.0f,  1.0f, -1.0f,
         -1.0f,  1.0f,  1.0f
    };

};