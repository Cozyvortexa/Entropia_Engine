#pragma once

#include <glad/glad/glad.h>
#include <GLFW/glfw3.h>

#include <algorithm>

#include "Render/Texture.h"
#include "Render/Shader.h"
#include "Render/Material.h"

#include "ECS/Resources/Resource.h"
#include "ECS/AssetStore.h"
#include "Render/Binding_Points.h"

#include <variant>

static struct MeshCorrespondence {
    uint32_t meshIndex = 0;

    size_t vertexStart;
    size_t vertexCount;

    size_t indexStart;
    size_t indexCount;
};

struct DrawElementsIndirectCommand {
    uint32_t count;         // Number of mesh indices
    uint32_t instanceCount; // Instance number
    uint32_t firstIndex;    // Début des indices dans l'EBO global
    int32_t  baseVertex;    // Début des sommets dans le VBO global
    uint32_t baseInstance;  // Index de départ de SES matrices dans le SSBO d'instances
};

// Structure alignée pour le SSBO (mémoire GPU)
struct InstanceData {
    glm::mat4 modelMatrix;

    // Handles Bindless (64-bit uint)
    uint64_t diffuseTexHandle;
    uint64_t normalTexHandle;
    uint64_t aoTexHandle;
    uint64_t roughnessTexHandle;
    uint64_t metallicTexHandle;

    float aoFactor;
    float roughnessFactor;
    float metallicFactor;
    uint32_t hasRoughness;
    uint32_t hasMetallic;
    uint32_t hasNormalMap;

};

static struct RenderCommand {
    uint32_t instanceNbr = 0;
    std::vector<InstanceData> instanceData;
};

class Renderer {
public:
	Renderer(AssetStore* assetStore) { 
		this->assetStore = assetStore; 
        vertexList.reserve(3000);
	}
    virtual ~Renderer() = default;

	//virtual void DrawMesh(Mesh& currentMesh, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, const glm::mat4& modelMatrix) = 0;
	virtual void DrawMesh_Without_Texture(Mesh& currentMesh) = 0;

	virtual void DrawQuad(Engine::Resource::RenderResource* renderData) = 0;

	virtual void DrawCube() = 0;

    virtual void SetViewport_Size(glm::vec2 newSize) = 0;

    virtual void SetupAxisArrow() = 0;

    virtual void RenderAxisGizmo(glm::vec3 objectPosition, glm::mat4 view, glm::mat4 projection, Shader& gizmoShader) = 0;

    virtual void ResizeFrameBufferText(Engine::Resource::RenderResource* renderData) = 0;

    virtual void AddMeshToRender(Engine::Component::MeshHandle newMesh) = 0;

    virtual void OrderDraw(Engine::Component::MeshHandle meshHandle, glm::mat4 modelMatrix) = 0;

    virtual  void ExecuteRenderCommands() = 0;

    void ClearVertexList();

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
    std::unordered_map<uint32_t, MeshCorrespondence> mesh_In_VertexList;  // MeshIndex to Correspondence
    std::vector<Vertex> vertexList;
    std::vector<uint32_t> indexList;
    std::unordered_map<uint32_t, std::vector<glm::mat4>> recordedMeshInstances; // Key : meshIndex
};

class OpenGL_Renderer : public Renderer{
public:
    OpenGL_Renderer(AssetStore* assetStore) : Renderer(assetStore) {
        LoadDefaultCube();
        SetupAxisArrow();

        glGenVertexArrays(1, &globalVAO);
        glGenBuffers(1, &globalVBO);
        glGenBuffers(1, &globalEBO);

        glBindVertexArray(globalVAO);

        glBindBuffer(GL_ARRAY_BUFFER, globalVBO);
        // Optionnel pour l'instant : on alloue une taille de départ (ex: 50 Mo pour les sommets)
        glBufferData(GL_ARRAY_BUFFER, 100 * 1024 * 1024, nullptr, GL_STATIC_DRAW);

        // Layout 0 : Position
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Position));
        // Layout 1 : Normal
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
        // Layout 2 : UV/TexCoords
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
        // Layout 3 : Tangent
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));


        // On lie le gros EBO (Ta indexList)
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, globalEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 100 * 1024 * 1024, nullptr, GL_STATIC_DRAW); // ex: 20 Mo d'indices

        glBindVertexArray(0);

        //IndirectBuffer
        glGenBuffers(1, &indirectBuffer);
        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBuffer);
        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);


        //Instance SSBO
        const size_t MAX_INSTANCES = 100000;
        size_t instanceBufferSize = MAX_INSTANCES * sizeof(InstanceData);

        glGenBuffers(1, &instanceSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, instanceSSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER, instanceBufferSize, NULL, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
        glBindBufferRange(GL_SHADER_STORAGE_BUFFER, INSTANCE_BUFFER_BINDING_POINT, instanceSSBO, 0, instanceBufferSize);

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, INSTANCE_BUFFER_BINDING_POINT, instanceSSBO);
    }

    void AddMeshToRender(Engine::Component::MeshHandle newMesh) override;

    void OrderDraw(Engine::Component::MeshHandle meshHandle, glm::mat4 modelMatrix) override;

    void ExecuteRenderCommands() override;

    //void DrawMesh(Mesh& currentMesh, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, const glm::mat4& modelMatrix) override;
    
    void DrawMesh_Without_Texture(Mesh& currentMesh) override;

    void DrawQuad(Engine::Resource::RenderResource* renderData) override;

    void DrawCube() override;

    void LoadDefaultCube();

    void SetViewport_Size(glm::vec2 newSize) override;

    void SetupAxisArrow() override;

    void RenderAxisGizmo(glm::vec3 objectPosition, glm::mat4 view, glm::mat4 projection, Shader& gizmoShader) override;

    void ResizeFrameBufferText(Engine::Resource::RenderResource* renderData) override;

    unsigned int cubeVAO = -1;
    unsigned int cubeVBO = -1;

    unsigned int arrowVAO = -1;
    unsigned int arrowVBO = -1;

    //Indirect
    unsigned int globalVAO = -1;
    unsigned int globalVBO = -1;
    unsigned int globalEBO = -1;

    unsigned int instanceSSBO = -1;
    unsigned int indirectBuffer = -1;
};