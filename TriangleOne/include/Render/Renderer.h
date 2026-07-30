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

static struct SubMeshCorrespondence {
    size_t vertexStart;
    size_t vertexCount;

    size_t indexStart;
    size_t indexCount;
};

static struct MeshCorrespondence {
    std::weak_ptr<Mesh> meshPtr;
    std::vector<SubMeshCorrespondence> subMesh_Correspondence;
};

struct DrawElementsIndirectCommand {
    uint32_t count;         // Number of mesh indices
    uint32_t instanceCount; // Instance number
    uint32_t firstIndex;    // Start of the indices in the global EBO
    int32_t  baseVertex;    // Start of the summits in the Global VBO
    uint32_t baseInstance;  // Starting index of its matrices in the instance SSBO
};

// Structure aligned in GPU-side memory
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

struct ShadowInstanceData {
    glm::mat4 modelMatrix;
    //Opacity purpose
    uint64_t diffuseTexHandle;
    uint64_t _pad0;
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
	virtual void ExecuteRenderCommands_Shadow() = 0;
	virtual void ExecuteRenderCommands_PointShadow() = 0;

	virtual void DrawQuad(Engine::Resource::RenderResource* renderData) = 0;

	virtual void DrawCube() = 0;

    virtual void SetViewport_Size(glm::vec2 newSize) = 0;

    virtual void SetViewport(glm::vec2 startPos, glm::vec2 newSize) = 0;

    virtual void SetupAxisArrow() = 0;

    virtual void RenderAxisGizmo(glm::vec3 objectPosition, glm::mat4 view, glm::mat4 projection, Shader& gizmoShader) = 0;

    virtual void ResizeFrameBufferText(Engine::Resource::RenderResource* renderData) = 0;

    // If the object is not found in the global vertex list, add it
    virtual MeshCorrespondence* AddMeshToRender(Engine::Component::MeshHandle newMesh) = 0;

    virtual void OrderDraw(Engine::Component::MeshHandle meshHandle, glm::mat4 modelMatrix) = 0;

    virtual void BuildInstance_ShadowSSBO() = 0;

    virtual void OrderShadowDraw(Engine::Component::MeshHandle meshHandle, glm::mat4 modelMatrix) = 0;

    virtual void ExecuteRenderCommands() = 0;

    void ClearVertexList();
    void ClearOrderList();
    void ClearShadowOrderList();

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
    const size_t MAX_INSTANCES = 100000;
protected: 
    AssetStore* assetStore;
    std::unordered_map<uint32_t, MeshCorrespondence> mesh_In_VertexList;  // Unique index mesh to his correspondence
    std::vector<Vertex> vertexList;
    std::vector<uint32_t> indexList;
    std::unordered_map<MeshCorrespondence*, std::vector<glm::mat4>> recordedMeshInstances;
    std::unordered_map<MeshCorrespondence*, std::vector<glm::mat4>> recordedMeshInstances_ShadowPass;


    std::vector<DrawElementsIndirectCommand> shadow_IndirectCommands;
    std::vector<DrawElementsIndirectCommand> pointShadow_IndirectCommands;
};

class OpenGL_Renderer : public Renderer{
public:
    OpenGL_Renderer(AssetStore* assetStore);

    // If the object is not found in the global vertex list, add it
    MeshCorrespondence* AddMeshToRender(Engine::Component::MeshHandle newMesh) override;

    void OrderDraw(Engine::Component::MeshHandle meshHandle, glm::mat4 modelMatrix) override;

    void OrderShadowDraw(Engine::Component::MeshHandle meshHandle, glm::mat4 modelMatrix) override;

    void BuildInstance_ShadowSSBO() override;

    void ExecuteRenderCommands() override;

    //void DrawMesh(Mesh& currentMesh, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, const glm::mat4& modelMatrix) override;
    
    void ExecuteRenderCommands_Shadow() override;
    void ExecuteRenderCommands_PointShadow() override;

    void DrawQuad(Engine::Resource::RenderResource* renderData) override;

    void DrawCube() override;

    void LoadDefaultCube();

    void SetViewport_Size(glm::vec2 newSize) override;

    void SetViewport(glm::vec2 startPos, glm::vec2 newSize) override;

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

    //Shadow
    unsigned int shadowIndirectBuffer = -1;
    unsigned int pointShadowIndirectBuffer = -1;

    unsigned int shadowInstanceSSBO = -1;
};