#pragma once
#include <glad/glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>
#include "Render/Shader.h"

#include <Audio/AudioStruct.h>
#include "Physics/PhysicsStruct.h"
#include "ECS/Components/BasicComponent.h"

#include "Utilities/ImGui/imgui.h"

#include "Physics/JoltRenderer.h"
#include "Render/RenderObject.h"

namespace Engine::Resource {

	struct Resource {
		Resource() = default;
		virtual ~Resource() = default;

		Resource(const Resource&) = delete;	// Pas de copie
		Resource& operator=(const Resource&) = delete;	// Pas d'affectation
	};

	struct WindowResource : public Resource {
		WindowResource() = default;

		inline static int WIDTH = 1920;
		inline static int HEIGHT = 1000;

		inline static bool isIconified = false;

		GLFWwindow* window = nullptr;
	};

	struct TimeResource : public Resource {
		float deltaTime = 0.0f;
		float lastFrame = 0.0f;

		float currentFps;
	};

	struct R_Shader : public Resource {
		std::unique_ptr<Shader> depthShader = nullptr;
		std::unique_ptr<Shader> depthShader_SpotLight = nullptr;
		std::unique_ptr<Shader> depthShaderCubeMap = nullptr;
		std::unique_ptr<Shader> postProcessShader = nullptr;
		std::unique_ptr<Shader> bloomShader = nullptr;
		std::unique_ptr<Shader> lightningPass_Shader = nullptr;
		std::unique_ptr<Shader> ssaoPass_Shader = nullptr;
		std::unique_ptr<Shader> ssaoPass_Blur_Shader = nullptr;
		std::unique_ptr<Shader> equirectangular_To_CubemapShader = nullptr;
		std::unique_ptr<Shader> skyBox_Shader = nullptr;
		std::unique_ptr<Shader> irradiance_Shader = nullptr;
		std::unique_ptr<Shader> prefilter_Shader = nullptr;
		std::unique_ptr<Shader> brdf_Shader = nullptr;
		std::unique_ptr<Shader> axisShader = nullptr;
	};

	struct GBuffer : public Resource {
		unsigned int gBuffer;
		unsigned int gPosition;
		unsigned int gNormal;
		unsigned int gAlbedo;
		unsigned int gARM;
		unsigned int gDepth;
	};

	struct SSAO : public Resource {
		unsigned int ssaoBuffer;
		unsigned int ssaoBlur_Buffer;
		unsigned int ssaoText;
		unsigned int ssaoBlurText;
		unsigned int ssao_NoiseText;

		bool ssao_Enabled = true;
		int kernelSample = 8;
		float SSAO_radius = 0.5f;
		std::vector<glm::vec3> ssaoKernel;
	};


	struct RenderResource : public Resource {
		int renderWIDTH = 1080;
		int renderHEIGHT = 1920;
		unsigned int mainMaterialHandle;
		unsigned int main_Instanced_Material_Handle;

		R_Shader r_Shader;

		glm::mat4 _model = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(0);

		//Lightning
		unsigned int framebuffer;
		GLuint finalTxtColorOutput[2];
		GLuint finalDepthOutput;

		//Shadow
		unsigned int dummyDepthMap2D = 0;
		unsigned int dummyDepthCubeMap = 0;

		GBuffer gBuffer;

		SSAO ssao;

		//Light SSBO
		All_Light lights;
		std::vector<size_t> lightSSBO_Data_Size;
		GLuint light_SSBO;

		//Final render
		unsigned int quadVAO;
		unsigned int quadVBO;
		//Bloom
		unsigned int pingpongFBO[2];
		unsigned int pingpongBuffers[2];
		bool horizontal = true;

		//Capture cubeMap / IBL
		int skyboxResolution = 512;
		unsigned int captureFBO;
		unsigned int captureRBO;
		unsigned int envCubemap;
		unsigned int irradianceMap;

		unsigned int prefilterMap;
		unsigned int maxMipLevels = 5;
		int specularResolution_Map = 128;
		unsigned int brdfLUTTexture;

		//Final render (To ImGui)
		unsigned int toImGui_FBO;
		unsigned int toImGui_Texture;

		////////////////Parameters
		float exposure = 0.5f; // HDR exposure

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

	struct ShadowResource : public Resource {
		//Shadow Array
		unsigned int point_Array[3];
		unsigned int spot_Array[3];

		//Shadow FBO
		unsigned int point_FBO[3];
		unsigned int spot_FBO[3];

		//Shadow SSBO
		uint32_t SSBO;
		uint32_t Text_SSBO;
		std::vector<size_t> SSBO_Data_Size;

		//Bindless Array
		uint64_t bindless_SpotArray[3];
		uint64_t bindless_PointArray[3];

		//Shadow unique DirLight
		unsigned int dirLight_depthMapFBO = 0;
		unsigned int dirLight_depthMap = 0;  // No bindless for the dir light
		Engine::Render::Shadow_Quality dirLight_Current_Quality = Engine::Render::Shadow_Quality::Low;
	};

	enum RenderTarget {
		Default,
		Albedo,
		Position,
		Normal,
		Depth,
		AmbientOcclusion,
		Roughness,
		Metallic,
		Irradiance_Map
	};

	enum class FileType {
		Directory,
		Audio,
		Video,
		Image,
		Model,
		Other
	};

	struct Node {
		std::string name;
		std::string path;
		std::string relativePath;
		FileType type;

		Node* parent = nullptr;
		std::vector<std::unique_ptr<Node>> children;
	};

	struct InterfaceRessource : public Resource {
		bool editorActive = true;
		bool mainInterfaceOpen = true;
		bool hierarchy_menu = true;
		bool renderWindowsToggle = true;
		bool inspecteur_Toogle = true;
		bool arbo_Toogle = true;
		RenderTarget renderTarget = RenderTarget::Default;
		glm::vec2 renderWindows_Size;

		ImVec2 previousSize;
		bool OnEditorView = false;

		uint32_t focusGameObject = Engine::Component::INVALIDE_uint32_t;

		std::unique_ptr<Node> mainDirectory = nullptr;
		Node* focusDirectory = nullptr;
	};

	struct InputResource : public Resource {
		bool mouseInputEnable;
	};

	struct ActiveCamera : public Resource {
		uint32_t cameraID;
	};

	struct AudioResource : public Resource {
		ma_engine audioEngine{};
		ma_resource_manager resourceManager;
	};

	struct PhysicsResource : public Resource {
		bool display_physicsShape = false;

		const uint64_t cMaxBodies = 65536;
		const uint64_t cNumBodyMutexes = 0;
		const uint64_t cMaxBodyPairs = 65536;
		const uint64_t cMaxContactConstraints = 1024;

		Engine::Physics::BPLayerInterfaceImpl broad_phase_layer_interface;

		Engine::Physics::ObjectVsBroadPhaseLayerFilterImpl object_vs_broadphase_layer_filter;

		Engine::Physics::ObjectLayerPairFilterImpl object_vs_object_layer_filter;

		JPH::PhysicsSystem physics_system;

		Engine::Physics::MyBodyActivationListener body_activation_listener;
		Engine::Physics::MyContactListener contact_listener;

		std::unique_ptr<JPH::JobSystemThreadPool> job_system;

		std::unique_ptr <JPH::TempAllocatorImpl> temp_allocator;

		const float physicsDeltaTime = 1.0f / 60.0f;
		float timeAccumulator = 0.0f;

		std::unique_ptr<Engine::Physics::JoltDebugRenderer> joltDebugRenderer = nullptr;
		std::unique_ptr<Shader> debugJoltShader = nullptr;
		//JPH::BodyManager::DrawSettings debug_draw_settings;
	};


	struct ResourceBuffer {
		WindowResource* windowResource;
		TimeResource* timeResource;
		RenderResource* renderResource;
		ActiveCamera* activeCamera;
		InputResource* inputResource;
		InterfaceRessource* interfaceRessource;
		AudioResource* audioResource;
		PhysicsResource* physicsResource;
		ShadowResource* shadowResource;
	};
}