//#pragma once
//#include <glad/glad/glad.h>
//#include <GLFW/glfw3.h>
//
//#include "ECS/System.h"
//
//#include <iostream>
//#include <vector>
//
//#include "Shader.h"
//#include "window.h"
//#include "Systemes/TimeSysteme/Time.hpp"
//#include "Texture.h"
//#include "Camera.h"
//
//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/type_ptr.hpp>
//
//#include "ModelClass.h"
//
//
//#include "Scene.h"
//#include "Entity/Entity.h"
//#include "Entity/Components/Light.h"
//#include "Entity/Systemes/RenderSystem.h"
//
//
//
//class RenderModule : public System {
//public:
//	RenderModule();
//
//	static RenderModule* GetInstance();
//
//	void DrawMirorCube();
//	void DrawTextureOnScreen();
//
//	void InitQuadVao();
//
//	void InitSkyBox();
//	void DrawSkyBox(glm::mat4 projectionMatrix);
//
//	void Init() override;
//	void Update(World& world) override;
//	void Shutdown(World& world) override;
//
//private:
//	std::shared_ptr<Shader> mainShader = nullptr;
//	std::shared_ptr<Shader> depthShader = nullptr;
//
//	std::shared_ptr<Shader> depthShaderCubeMap = nullptr;
//	//A delette
//	Shader* ppShader = nullptr;
//	Shader* skyboxShader = nullptr;
//	Shader* reflectShader = nullptr;
//	//
//
//
//	GLFWwindow* window = nullptr;
//	Camera* mainCamera = nullptr;
//
//	Light* dirLight = nullptr;
//
//	static RenderModule* instance;
//
//	std::vector<unsigned int> shaderListe;
//
//	glm::mat4 _model = glm::mat4(1.0f);
//
//
//	std::vector<glm::vec3> pointLightPositions;
//
//	int sample = 4;
//
//	unsigned int framebuffer;
//	GLuint finalTxtColorOutput;
//	GLuint finalTxtOutput;
//
//	//Final render
//	unsigned int quadVAO;  
//	unsigned int quadVBO;  
//
//
//	RenderSystem* renderSystem = nullptr;
//	Scene* currentScene = nullptr;
//
//	//Skybox
//	unsigned int cubemapTexture;
//	unsigned int skyboxVAO;
//	unsigned int skyboxVBO;
//
//	//Legacy
//	//Reflect
//	unsigned int reflectVAO;
//	unsigned int reflectVBO;
//};