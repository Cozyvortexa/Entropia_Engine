#pragma once
#include <glad/glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Render/Shader.h"

#include "ECS/SpareSet.h"

struct Component {
	virtual ~Component() = default;
};

struct Transform : public Component
{
	Transform() {};
	Transform(glm::vec3 position) { this->position = position; }
	Transform(glm::vec3 position, glm::vec3 rotation) { this->position = position; this->rotation = rotation; }
	Transform(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale) { this->position = position; this->rotation = rotation; this->scale = scale; }
	glm::vec3 position = glm::vec3(0.0f);
	glm::vec3 rotation = glm::vec3(1.0f);
	glm::vec3 scale = glm::vec3(1.0f);

	glm::mat4 GetTransformModel() const{
		glm::mat4 model(1.0f);


		model = glm::translate(model, position);

		model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1, 0, 0));
		model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0, 1, 0));
		model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0, 0, 1));

		model = glm::scale(model, glm::vec3(scale));

		return model;
	}
};

struct CameraComponent : public Component {
	CameraComponent(unsigned int SCR_WIDTH, unsigned int SCR_HEIGHT) {
		this->SCR_WIDTH = SCR_WIDTH;
		this->SCR_HEIGHT = SCR_HEIGHT;
		lastX = SCR_WIDTH / 2.0f;
		lastY = SCR_HEIGHT / 2.0f;
	}
	glm::mat4 projection = glm::mat4(1.0f);
	glm::mat4 model = glm::mat4(1.0f);
	glm::vec3 direction = glm::vec3(0.0f, 0.0f, 0.0f);

	float yoffset = 0.0f;

	glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
	glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

	float nearPlane = 0.1f;
	float farPlane = 200.0f;

	float zoom = 45.0f;  // valeur de zoom par default 
	//Rotation
	float yaw = -90.0f;
	float pitch = 0.0f;

	bool firstMouse = true;

	unsigned int SCR_WIDTH = 0;
	unsigned int SCR_HEIGHT = 0;

	float lastX = 1.0f;
	float lastY = 1.0f;

	glm::mat4 viewMatrice = glm::mat4(1.0f);  // Juste pour pas qu'il soit init a zero
};

struct SceneTag : public Component {
	uint32_t scene_id = 0;  // Zero correspond to the main scene 
};

struct MeshHandle : public Component {
	MeshHandle() {};
	MeshHandle(uint32_t index) { this->index = index; }
	MeshHandle(uint32_t index, bool castShadow, bool haveToBeDraw = true) { this->index = index; this->castShadow = castShadow; this->haveToBeDraw = haveToBeDraw; }
	uint32_t index = 0;
	bool castShadow = true;
	bool haveToBeDraw = true;
};

struct MaterialHandle : Component {
	MaterialHandle() {};
	MaterialHandle(uint32_t index) { this->index = index; }
	uint32_t index = 0;
};

enum LightTag {
	None,
	Directional_Tag,
	PointLight_Tag,
	SpotLight_Tag
};
struct LightToInitTag : public Component {
	LightToInitTag() {};
	LightToInitTag(LightTag tag) { this->tag = tag; }
	LightTag tag = LightTag::None;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct Resource {
	Resource() = default;
	virtual ~Resource() = default;

	Resource(const Resource&) = delete;	// Pas de copie
	Resource& operator=(const Resource&) = delete;	// Pas d'affectation
};

struct WindowResource : public Resource {
	WindowResource() = default;

	inline static int WIDTH = 800;
	inline static int HEIGHT = 600;

	int sample = 4;

	GLFWwindow* window = nullptr;
};

struct TimeResource : public Resource {
	float deltaTime = 0.0f;
	float lastFrame = 0.0f;

};

struct RenderResource : public Resource {
	unsigned int mainMaterialHandle;
	std::unique_ptr<Shader> depthShader = nullptr;
	std::unique_ptr<Shader> depthShaderCubeMap = nullptr;
	std::unique_ptr<Shader> postProcessShader = nullptr;
	std::unique_ptr<Shader> bloomShader = nullptr;
	std::unique_ptr<Shader> lightningPass_Shader = nullptr;
	std::unique_ptr<Shader> ssaoPass_Shader = nullptr;
	std::unique_ptr<Shader> ssaoPass_Blur_Shader = nullptr;

	glm::mat4 _model = glm::mat4(1.0f);

	int sample = 4;

	glm::mat4 projection = glm::mat4(0);

	//Lightning
	unsigned int framebuffer;
	GLuint finalTxtColorOutput[2];
	GLuint finalTxtOutput;

	////Intermediate
	unsigned int intermediateFBO;

	//Shadow
	unsigned int dummyDepthMap2D = 0;
	unsigned int dummyDepthCubeMap = 0;

	////Deffered - GBuffer
	unsigned int gBuffer;
	unsigned int gPosition;
	unsigned int gNormal;
	unsigned int gAlbedo;

	unsigned int gDepth;
	//Deffered resolved Text (MSAA)
	unsigned int gPositionResolved;
	unsigned int gNormalResolved;
	unsigned int gAlbedoResolved;
	unsigned int gDepthResolved;

	//SSAO
	unsigned int ssaoBuffer;
	unsigned int ssaoBlur_Buffer;

	unsigned int ssaoText;
	unsigned int ssaoBlurText;
	unsigned int ssao_NoiseText;

	bool ssao_Enabled = true;
	int kernelSample = 8;
	std::vector<glm::vec3> ssaoKernel;

	//Light SSBO
	std::vector<size_t> lightSSBO_Data_Size;
	GLuint light_SSBO;

	//Final render
	unsigned int quadVAO;
	unsigned int quadVBO;
	//Bloom
	unsigned int pingpongFBO[2];
	unsigned int pingpongBuffers[2];
	bool horizontal = true;

	////////////////Parameters
	float exposure = 1.0f; // HDR exposure

	//Bloom
	bool bloomEnable = true;
	int bloom_iteration = 10;

	float quadVertices[24] = {
		// Position      // Text
		-1.0f,  1.0f,     0.0f, 1.0f,
		 1.0f, -1.0f,     1.0f, 0.0f,
		-1.0f, -1.0f,     0.0f, 0.0f,

		-1.0f,  1.0f,     0.0f, 1.0f,
		 1.0f,  1.0f,     1.0f, 1.0f,
		 1.0f, -1.0f,     1.0f, 0.0f
	};
};

struct ActiveCamera : public Resource {
	Entity cameraID;
};

struct ResourceBuffer {
	WindowResource* windowResource;
	TimeResource* timeResource;
	RenderResource* renderResource;
	ActiveCamera* activeCamera;
};