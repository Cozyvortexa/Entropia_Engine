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
	void FactoDirLight(Shader* lightShader, glm::vec3 worldLightPos);

	void InitSkyBox();
	void DrawSkyBox(glm::mat4 projectionMatrix);

	void Init() override;
	void Render() override;
	void Shutdown() override;


private:
	Shader* shader = nullptr;
	Shader* shaderLight = nullptr;
	Shader* ppShader = nullptr;
	Shader* skyboxShader = nullptr;
	Shader* reflectShader = nullptr;
	GLFWwindow* window = nullptr;
	Camera* mainCamera = nullptr;

	static RenderModule* instance;

	std::vector<unsigned int> shaderListe;

	glm::mat4 _model = glm::mat4(1.0f);

	glm::vec3 lightPos;

	std::vector<glm::vec3> pointLightPositions;

	Model* modelMesh = nullptr;

	unsigned int framebuffer;
	GLuint finalTxtColorOutput;
	GLuint finalTxtOutput;

	unsigned int quadVAO;  
	unsigned int quadVBO;  

	unsigned int cubemapTexture;
	unsigned int skyboxVAO;
	unsigned int skyboxVBO;

	unsigned int reflectVAO;
	unsigned int reflectVBO;
};