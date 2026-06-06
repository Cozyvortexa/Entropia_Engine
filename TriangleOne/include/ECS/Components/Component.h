#pragma once
#include <glad/glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Render/Shader.h"

#include "ECS/SpareSet.h"

#include "Utilities/ImGui/imgui.h"

#include <ECS/Components/AudioStruct.h>
#include <Physics/PhysicsHelper.h>
#include "Audio/AudioHelper.h"

#include "Utilities/Observer.h"

class All_Light;
namespace Engine::Component {
	static const uint32_t INVALIDE_uint32_t = static_cast<uint32_t>(-1);
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


		template<typename F>
		void Reflect(F&& f)
		{
			f("Position:", position);
			f("Rotation:", rotation);
			f("Scale:", scale);
		}

		glm::mat4 GetTransformModel() const {
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
		glm::mat4 model = glm::mat4(1.0f);
		glm::vec3 direction = glm::vec3(0.0f, 0.0f, 0.0f);

		float cameraSpeed = 9.0f;

		float yoffset = 0.0f;

		glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
		glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
		glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

		float nearPlane = 0.1f;
		float farPlane = 200.0f;

		float zoom = 45.0f;  // Default zoom value 
		//Rotation
		float yaw = -90.0f;
		float pitch = 0.0f;

		bool firstMouse = true;

		unsigned int SCR_WIDTH = 0;
		unsigned int SCR_HEIGHT = 0;

		float lastX = 1.0f;
		float lastY = 1.0f;

		glm::mat4 viewMatrice = glm::mat4(1.0f);
	};

	struct SceneTag : public Component {
		SceneTag() = default;
		SceneTag(std::string name) { this->name = name; }
		uint16_t scene_id = 0;  // Zero correspond to the main scene 
		std::string name = "Default";
	};

	using MeshIndex = uint32_t;  // Named integer

	struct MeshHandle : public Component {
		MeshHandle() {};
		MeshHandle(uint32_t index) { this->index = index; }
		MeshHandle(uint32_t index, bool castShadow, bool haveToBeDraw = true) { this->index = index; this->castShadow = castShadow; this->haveToBeDraw = haveToBeDraw; }
		MeshIndex index = 0;
		bool castShadow = true;
		bool haveToBeDraw = true;

		template<typename F>
		void Reflect(F&& f)
		{
			f("HaveToBeDraw", haveToBeDraw);
			f("CastShadow", castShadow);
			f("Mesh", index);
		}
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

	struct AudioSource : public Component{
	public:
		~AudioSource() {
			Engine::Audio::DeleteSound(audio);
		}
		AudioSource() { 
			audio = new Engine::Audio::Audio();
			SetupConnections();
			volume.Set(1.0f);
			range.Set(10.0f);
		};
		AudioSource(std::string name, std::string path, Engine::Audio::Audio* audio) {
			this->name = name;
			this->path = path;
			this->audio = audio;

			SetupConnections();
			volume.Set(1.0f);
			range.Set(10.0f);
		}

		AudioSource(AudioSource&& other) noexcept: name(std::move(other.name)), path(std::move(other.path)), audio(std::exchange(other.audio, nullptr)) {
			volume.Set(other.volume.Get());
			range.Set(other.range.Get());

			other.audio = nullptr;

			SetupConnections();
		}
		AudioSource& operator=(AudioSource&& other) noexcept {
			if (this != &other) {

				if (this->audio) {
					Engine::Audio::DeleteSound(this->audio);
				}
				//Clear current connection
				volumeConnection = ScopedConnection();
				rangeConnection = ScopedConnection();

				//Transfers the master data
				name = std::move(other.name);
				path = std::move(other.path);

				audio = (std::exchange(other.audio, nullptr));

				//Synchronises the values of the observers
				volume.Set(other.volume.Get());
				range.Set(other.range.Get());

				SetupConnections();
			}
			return *this;
			}

		// Copy is prohibidden
		AudioSource(const AudioSource&) = delete;
		AudioSource& operator=(const AudioSource&) = delete;


		std::string path = "";
		std::string name = "None";
		Engine::Audio::Audio* audio;

		Observer<float> volume;
		Observer<float> range;

		template<typename F>
		void Reflect(F&& f)
		{
			f("Volume", volume);
			f("range", range);
			f("Audio", audio);
		}

	private:
		void SetupConnections() {
			volumeConnection = volume.Subscribe([this](const float& v) {
				if (this->audio) Audio::SetVolume(*(this->audio), v);
				});
			rangeConnection = range.Subscribe([this](const float& v) {
				if (this->audio) Audio::SetMaxDistance(*(this->audio), v);
				});
		}

		ScopedConnection volumeConnection;
		ScopedConnection rangeConnection;
	};

	struct AudioListener : public Component{
		ma_uint32 index = 0;
	};

	struct PhysicObject : public Component {
	public:
		inline JPH::BodyID& GetBodyID() { return bodyID; }
	protected:
		JPH::BodyID bodyID;
	};

	struct BoxCollider : public PhysicObject {
		BoxCollider() = delete;
		BoxCollider(glm::vec3 position) {
			boxSize.Set(glm::vec3(1.0));
			motionType.Set(JPH::EMotionType::Dynamic);
			//gravity.Set(false);
			bodyID = Engine::Physics::PhysicsHelper::CreateBox(position, boxSize.Get(), JPH::Quat::sIdentity(), motionType.Get());

			//Physics::PhysicsHelper::SetGravity(bodyID, gravity.Get());
			Physics::PhysicsHelper::AddBody_To_SimulateWorld(bodyID);
		}
		BoxCollider(glm::vec3 position, glm::vec3 size) {
			boxSize.Set(size);
			motionType.Set(JPH::EMotionType::Dynamic);
			//gravity.Set(false);
			bodyID = Engine::Physics::PhysicsHelper::CreateBox(position, boxSize.Get(), JPH::Quat::sIdentity(), motionType.Get());

			//Physics::PhysicsHelper::SetGravity(bodyID, gravity.Get());
			Physics::PhysicsHelper::AddBody_To_SimulateWorld(bodyID);
		}

		~BoxCollider() {
			if (bodyID.IsInvalid()){
				Engine::Physics::PhysicsHelper::DeleteBody(bodyID);
			}
		}

		BoxCollider(BoxCollider&& other) noexcept{
			boxSize.Set(other.boxSize.Get());
			motionType.Set(other.motionType.Get());
			gravity.Set(other.gravity.Get());
			bodyID = other.bodyID;

			SetupConnections();
		}
		BoxCollider& operator=(BoxCollider&& other) noexcept {
			if (this != &other) {
				//Clear current connection
				boxSize_Connection = ScopedConnection();
				motionType_Connection = ScopedConnection();
				gravity_Connection = ScopedConnection();

				//Transfers the master data
				bodyID = other.bodyID;
				offset = std::move(other.offset);

				//Synchronises the values of the observers
				boxSize.Set(other.boxSize.Get());
				motionType.Set(other.motionType.Get());
				gravity.Set(other.gravity.Get());

				SetupConnections();
			}
			return *this;
		}
		BoxCollider(const BoxCollider&) = delete;
		BoxCollider& operator=(const BoxCollider&) = delete;


		glm::vec3 offset = glm::vec3(0);
		Observer<glm::vec3> boxSize;

		Observer<bool> gravity;

		Observer<JPH::EMotionType> motionType;

		template<typename F>
		void Reflect(F&& f)
		{
			f("Gravity", gravity);
			f("boxSize:", boxSize);
			f("Offset", offset);
			f("Motion Type:", motionType);
		}
	private:
		void SetupConnections() {
			boxSize_Connection = boxSize.Subscribe([this](const glm::vec3& vec) {
				bodyID = Engine::Physics::PhysicsHelper::ResizeBox(bodyID, vec);
				});
			motionType_Connection = motionType.Subscribe([this](const JPH::EMotionType& value) {
				Engine::Physics::PhysicsHelper::SetMotionType(bodyID, value);
				});
			gravity_Connection = gravity.Subscribe([this](const bool& value) {
				Engine::Physics::PhysicsHelper::SetGravity(bodyID, value);
				});
		}

		ScopedConnection boxSize_Connection;
		ScopedConnection motionType_Connection;
		ScopedConnection gravity_Connection;
	};
}


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
		int kernelSample = 16;
		float SSAO_radius = 0.2f;
		std::vector<glm::vec3> ssaoKernel;
	};


	struct RenderResource : public Resource {
		int renderWIDTH;
		int renderHEIGHT;
		unsigned int mainMaterialHandle;

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
		All_Light* lights = nullptr;
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
		bool bloomEnable = false;
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

		uint32_t focusGameObject = Component::INVALIDE_uint32_t;

		std::unique_ptr<Node> mainDirectory = nullptr;
		Node* focusDirectory = nullptr;
	};

	struct InputResource : public Resource {
		bool mouseInputEnable;
	};

	struct ActiveCamera : public Resource {
		Entity cameraID;
	};

	struct AudioResource : public Resource {
		ma_engine audioEngine{};
		ma_resource_manager resourceManager;
	};

	struct PhysicsResource : public Resource{
		// This is the max amount of rigid bodies that you can add to the physics system. If you try to add more you'll get an error.
		const uint64_t cMaxBodies = 65536;

		// This determines how many mutexes to allocate to protect rigid bodies from concurrent access. Set it to 0 for the default settings.
		const uint64_t cNumBodyMutexes = 0;

		// This is the max amount of body pairs that can be queued at any time (the broad phase will detect overlapping
		// body pairs based on their bounding boxes and will insert them into a queue for the narrowphase). If you make this buffer
		// too small the queue will fill up and the broad phase jobs will start to do narrow phase work. This is slightly less efficient.
		const uint64_t cMaxBodyPairs = 65536;

		// This is the maximum size of the contact constraint buffer. If more contacts (collisions between bodies) are detected than this
		// number then these contacts will be ignored and bodies will start interpenetrating / fall through the world.
		// Note: This value is low because this is a simple test. For a real project use something in the order of 10240.
		const uint64_t cMaxContactConstraints = 1024;

		Engine::Physics::BPLayerInterfaceImpl broad_phase_layer_interface;

		Engine::Physics::ObjectVsBroadPhaseLayerFilterImpl object_vs_broadphase_layer_filter;

		// Create class that filters object vs object layers
		// Note: As this is an interface, PhysicsSystem will take a reference to this so this instance needs to stay alive!
		// Also have a look at ObjectLayerPairFilterTable or ObjectLayerPairFilterMask for a simpler interface.
		Engine::Physics::ObjectLayerPairFilterImpl object_vs_object_layer_filter;

		// Now we can create the actual physics system.
		JPH::PhysicsSystem physics_system;

		Engine::Physics::MyBodyActivationListener body_activation_listener;
		Engine::Physics::MyContactListener contact_listener;

		std::unique_ptr<JPH::JobSystemThreadPool> job_system;

		std::unique_ptr <JPH::TempAllocatorImpl> temp_allocator;
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
	};
}
