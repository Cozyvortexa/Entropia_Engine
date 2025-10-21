#pragma once
#include <glad/glad/glad.h>
#include <GLFW/glfw3.h>

#include "Module.h"

#include <iostream>
#include <vector>

#include "Shader.h"
#include "window.h"
#include "Time.hpp"
#include "Texture.h"
#include "Camera.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ModelClass.h"

#include "Light.h"
#include "GameObject.h"



class RenderModule : public Module {
public:
	RenderModule();

	static RenderModule* GetInstance();

	void DrawLight(int indice);
	void DrawMirorCube();
	void DrawTextureOnScreen();

	void InitQuadVao();

	void FactoPointLight(Shader* lightShader, int i);
	void FactoSpotLight(Shader* lightShader, int i);

	void InitSkyBox();
	void DrawSkyBox(glm::mat4 projectionMatrix);

	void InitShadowMap();

	void InitCubeMap();

	void DrawShadowMap();

	void DrawShadowPoint();


	void Init() override;
	void Render() override;
	void Shutdown() override;

private:
	Shader* _shader = nullptr;
	Shader* shaderLight = nullptr;
	Shader* ppShader = nullptr;
	Shader* skyboxShader = nullptr;
	Shader* reflectShader = nullptr;
	Shader* depthShader = nullptr;
	Shader* depthShaderCubeMap = nullptr;

	GLFWwindow* window = nullptr;
	Camera* mainCamera = nullptr;

	Light* dirLight = nullptr;

	static RenderModule* instance;

	std::vector<unsigned int> shaderListe;

	glm::mat4 _model = glm::mat4(1.0f);

	//glm::vec3 lightPos;

	std::vector<glm::vec3> pointLightPositions;

	GameObject* house = nullptr;
	GameObject* cube = nullptr;

	int sample = 4;

	unsigned int framebuffer;
	GLuint finalTxtColorOutput;
	GLuint finalTxtOutput;

	//Final render
	unsigned int quadVAO;  
	unsigned int quadVBO;  

	//Skybox
	unsigned int cubemapTexture;
	unsigned int skyboxVAO;
	unsigned int skyboxVBO;

	//Reflect
	unsigned int reflectVAO;
	unsigned int reflectVBO;

	//Shadow
	unsigned int depthMapFBO;
	unsigned int depthMap;
	const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;
	float far_plane = 30.0;
	//Shadow cube
	unsigned int depthCubeMapFBO;
	unsigned int depthCubemap;
};