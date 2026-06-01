#pragma once

#include <glad/glad/glad.h>
#include <GLFW/glfw3.h>

#include <algorithm>

#include "Render/Texture.h"
#include "Render/Shader.h"
#include "Render/Material.h"

#include "ECS/Components/Component.h"
#include "ECS/AssetStore.h"

#include <variant>

class Renderer {
public:
	Renderer(AssetStore* assetStore) { 
		this->assetStore = assetStore; 
	}
    virtual ~Renderer() = default;

	virtual void DrawMesh(Mesh& currentMesh) = 0;
	virtual void DrawMesh_Without_Texture(Mesh& currentMesh) = 0;

	virtual void DrawQuad(Engine::Resource::RenderResource* renderData) = 0;

	virtual void DrawCube() = 0;

    virtual void SetViewport_Size(glm::vec2 newSize) = 0;

    virtual void SetupAxisArrow() = 0;

    virtual void RenderAxisGizmo(glm::vec3 objectPosition, glm::mat4 view, glm::mat4 projection, Shader& gizmoShader) = 0;

    const float cubeVertices[108] = {
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
         1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         // bottom face
         -1.0f, -1.0f, -1.0f,
          1.0f, -1.0f, -1.0f,
          1.0f, -1.0f,  1.0f,
          1.0f, -1.0f,  1.0f,
         -1.0f, -1.0f,  1.0f,
         -1.0f, -1.0f, -1.0f,
         // top face
         -1.0f,  1.0f, -1.0f,
          1.0f,  1.0f, -1.0f,
          1.0f,  1.0f,  1.0f,
          1.0f,  1.0f,  1.0f,
         -1.0f,  1.0f,  1.0f,
         -1.0f,  1.0f, -1.0f,
    };
protected: 
    AssetStore* assetStore;
};

class OpenGL_Renderer : public Renderer{
public:
    OpenGL_Renderer(AssetStore* assetStore) : Renderer(assetStore) {
        LoadDefaultCube();
        SetupAxisArrow();
    }


    void DrawMesh(Mesh& currentMesh) override;
    
    void DrawMesh_Without_Texture(Mesh& currentMesh) override;

    void DrawQuad(Engine::Resource::RenderResource* renderData) override;

    void DrawCube() override;

    void LoadDefaultCube();

    void SetViewport_Size(glm::vec2 newSize) override;

    void SetupAxisArrow() override;

    void RenderAxisGizmo(glm::vec3 objectPosition, glm::mat4 view, glm::mat4 projection, Shader& gizmoShader) override;

    unsigned int cubeVAO = -1;
    unsigned int cubeVBO = -1;

    unsigned int arrowVAO = -1;
    unsigned int arrowVBO = -1;
};