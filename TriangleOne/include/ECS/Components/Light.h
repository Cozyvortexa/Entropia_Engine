#pragma once
#define NOMINMAX
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

#include "Systems/CameraSystem.h"
#include "Render/Shader.h"
#include "ECS/Components/Component.h"

struct AABB {
	AABB() { min = glm::vec3(0);  max = glm::vec3(0);};
	AABB(glm::vec3 newMin, glm::vec3 newMax)
	{
		min = newMin;
		max = newMax;
	}
public:
	glm::vec3 min;
	glm::vec3 max;
};

namespace Engine::Component {
	struct Light : Component {
	public:
		~Light() = default;
		Light(Shader* newDepthShader) { this->depthShader = newDepthShader; }
		Light(glm::vec3 color, float intensity);

		glm::vec3 color = glm::vec3(1.0f);
		float intensity = 150.0f;
		unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;
		Shader* depthShader = nullptr;


	};

	struct DirLight : public Light {
		using Light::Light;
		~DirLight() = default;
		DirLight() = default;
		DirLight(glm::vec3 color, float intensity, glm::vec3 direction, Shader* _depthShader);
		glm::vec3 direction = glm::vec3(0.0f, -0.80f, 0.0f);
		unsigned int depthMap = 0;
		unsigned int depthMapFBO = 0;

		//Shadow purpose
		float near_plane = 0.1f, far_plane = 100.0f;
		float orthoSize = 40.0f;

		glm::mat4 projection = glm::mat4(1);
		glm::vec3 lightPos = glm::vec3(0);
		glm::mat4 lightViewMatrice = glm::mat4(1);
		glm::mat4 lightMatrice = glm::mat4(1);

		template<typename F>
		void Reflect(F&& f)
		{
			f("Color", color);
			f("Intensity", intensity);
			f("Direction", direction);
		}

		glm::mat4 UpdateMatrix(const glm::mat4 viewMatrice, const glm::mat4 projectionCamera);

	private:
		glm::vec3 FrustumCenter(const std::vector<glm::vec3> corners);
		std::vector<glm::vec3> CalcWorldCorner(const glm::mat4 projection, glm::mat4 viewMatrice);
		AABB CalcBoundingBox(std::vector<glm::vec3> worldCorner);
		std::vector<glm::vec3> WorldCornerToLightSpace(glm::mat4 lightViewMatrice, std::vector<glm::vec3> worldCorners);

		std::vector<glm::vec3> ndcCubePoint;
	};

	struct PointLight : public Light {
	public:
		using Light::Light;
		~PointLight() = default;
		PointLight(glm::vec3 color, float intensity, float _range, Shader* depthShaderCubeMap);
		float range = 5.0f;

		unsigned int depthCubeMapFBO = 0;
		unsigned int depthCubeMap = 0;

		//Shadow purpose
		float aspect = 1.0f;
		float near_plane = 0.1f;

		template<typename F>
		void Reflect(F&& f)
		{
			f("Color", color);
			f("Intensity", intensity);
			f("Range", range);
		}
	};

	struct SpotLight : public Light {
	public:
		using Light::Light;
		~SpotLight() = default;
		SpotLight(glm::vec3 color, float intensity, glm::vec3 _direction, float _cutOff, float _outercutOff, float range, Shader* depthShaderSpotMap);
		float range = 1.0f;

		glm::vec3 direction = glm::vec3(1.0f, 0.0f, 0.0f);
		float cutOff = 5.5f;
		float outerCutOff = 15.5f;

		float aspect = 1.0f;

		template<typename F>
		void Reflect(F&& f)
		{
			f("Color", color);
			f("Intensity", intensity);
		}

		unsigned int depthMapFBO = 0;
		unsigned int depthMap = 0;

		glm::mat4 lightSpaceMatrix = glm::mat4(1.0f);
	};
}

