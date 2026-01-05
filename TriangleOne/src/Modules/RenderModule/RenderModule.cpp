#include <Modules/RenderModule/RenderModule.h>


float verticesLightRec[] = {
-0.5f, -0.5f, -0.5f, 
0.5f, -0.5f, -0.5f, 
0.5f, 0.5f, -0.5f, 
0.5f, 0.5f, -0.5f, 
-0.5f, 0.5f, -0.5f, 
-0.5f, -0.5f, -0.5f, 

-0.5f, -0.5f, 0.5f, 
0.5f, -0.5f, 0.5f, 
0.5f, 0.5f, 0.5f, 
0.5f, 0.5f, 0.5f, 
-0.5f, 0.5f, 0.5f, 
-0.5f, -0.5f, 0.5f, 

-0.5f, 0.5f, 0.5f, 
-0.5f, 0.5f, -0.5f, 
-0.5f, -0.5f, -0.5f, 
-0.5f, -0.5f, -0.5f, 
-0.5f, -0.5f, 0.5f, 
-0.5f, 0.5f, 0.5f, 

0.5f, 0.5f, 0.5f, 
0.5f, 0.5f, -0.5f, 
0.5f, -0.5f, -0.5f, 
0.5f, -0.5f, -0.5f, 
0.5f, -0.5f, 0.5f, 
0.5f, 0.5f, 0.5f, 

-0.5f, -0.5f, -0.5f, 
0.5f, -0.5f, -0.5f, 
0.5f, -0.5f, 0.5f, 
0.5f, -0.5f, 0.5f, 
-0.5f, -0.5f, 0.5f, 
-0.5f, -0.5f, -0.5f, 

-0.5f, 0.5f, -0.5f, 
0.5f, 0.5f, -0.5f,
0.5f, 0.5f, 0.5f, 
0.5f, 0.5f, 0.5f, 
-0.5f, 0.5f, 0.5f, 
-0.5f, 0.5f, -0.5f
};

float verticesNormal[] = {
	// positions         // normals
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, -1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f, 0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f, 0.0f, -1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 0.0f, -1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, -1.0f,

	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f, 0.0f,  1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f, 0.0f,  1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,  1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  1.0f,

	-0.5f,  0.5f,  0.5f, -1.0f, 0.0f,  0.0f,
	-0.5f,  0.5f, -0.5f, -1.0f, 0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f, -1.0f, 0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f, -1.0f, 0.0f,  0.0f,
	-0.5f, -0.5f,  0.5f, -1.0f, 0.0f,  0.0f,
	-0.5f,  0.5f,  0.5f, -1.0f, 0.0f,  0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, -1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, -1.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f, 0.0f,

	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f
};

float quadVertices[] = {
	// Position      // Text
	-1.0f,  1.0f,     0.0f, 1.0f,
	 1.0f, -1.0f,     1.0f, 0.0f,
	-1.0f, -1.0f,     0.0f, 0.0f,

	-1.0f,  1.0f,     0.0f, 1.0f,
	 1.0f,  1.0f,     1.0f, 1.0f,
	 1.0f, -1.0f,     1.0f, 0.0f
};

float skyboxVertices[] = {
	// positions          
	-1.0f,  1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	-1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f
};


std::vector<std::string> faces =
{
	"Assets/SkyBox/mountain/right.jpg",
	"Assets/SkyBox/mountain/left.jpg",
	"Assets/SkyBox/mountain/top.jpg",
	"Assets/SkyBox/mountain/bottom.jpg",
	"Assets/SkyBox/mountain/front.jpg",
	"Assets/SkyBox/mountain/back.jpg"
};


glm::vec3 cubePositions[] = {
	glm::vec3(0.0f, 0.0f, 0.0f),
	glm::vec3(2.0f, 5.0f, -15.0f),
	glm::vec3(-1.5f, -2.2f, -2.5f),
	glm::vec3(-3.8f, -2.0f, -12.3f),
	glm::vec3(2.4f, -0.4f, -3.5f),
	glm::vec3(-1.7f, 3.0f, -7.5f),
	glm::vec3(1.3f, -2.0f, -2.5f),
	glm::vec3(1.5f, 2.0f, -2.5f),
	glm::vec3(1.5f, 0.2f, -1.5f),
	glm::vec3(-1.3f, 1.0f, -1.5f)
};

RenderModule* RenderModule::instance = nullptr;

RenderModule::RenderModule() {
	instance = this;
}

RenderModule* RenderModule::GetInstance() {
	return instance;
}

void RenderModule::FactoPointLight(Shader* lightShader, int i) {
	lightShader->setVec3("pointLights[" + std::to_string(i) + "].position", pointLightPositions[i]);

	lightShader->setVec3("pointLights[" + std::to_string(i) + "].ambient", glm::vec3(0.2f, 0.2f, 0.2f));
	lightShader->setVec3("pointLights[" + std::to_string(i) + "].diffuse", glm::vec3(5.0f, 5.0f, 5.0f));
	lightShader->setVec3("pointLights[" + std::to_string(i) + "].specular", glm::vec3(0.5f, 0.5f, 0.5f));

	lightShader->setFloat("pointLights[" + std::to_string(i) + "].constant", 1.0f);
	lightShader->setFloat("pointLights[" + std::to_string(i) + "].linear", 0.09f);
	lightShader->setFloat("pointLights[" + std::to_string(i) + "].quadratic", 0.032f);
}

void RenderModule::FactoSpotLight(Shader* lightShader, int i) {
	lightShader->setVec3("spotLight.Position", glm::vec4(0.0f, 0.0f, -3.0f, 1.0f));

	lightShader->setVec3("spotLight.ambient", glm::vec3(0.5f, 0.5f, 0.5f));
	lightShader->setVec3("spotLight.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
	lightShader->setVec3("spotLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));

	lightShader->setFloat("spotLight.constant", 1.0f);
	lightShader->setFloat("spotLight.linear", 0.09f);
	lightShader->setFloat("spotLight.quadratic", 0.032f);

	lightShader->setVec3("spotLight.direction", glm::vec4(0.0f, 0.0f, 1.0f,0.0f));
	lightShader->setFloat("spotLight.cutOff", glm::cos(glm::radians(17.5f)));
	lightShader->setFloat("spotLight.outerCutOff", glm::cos(glm::radians(25.0f)));
}

//glm::mat4 RenderModule::DrawShadowDir() {
//	float near_plane = 1.0f, far_plane = 50.0f;
//	float orthoSize = 50.0f;
//	float distance = far_plane / 2;
//	glm::mat4 lightProjection = glm::ortho(-orthoSize, orthoSize, -orthoSize, orthoSize, near_plane, far_plane);
//
//	glm::vec3 lightPos = normalize(worldLightDir) * distance;
//
//	glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
//	
//	glm::mat4 lightMatrice = lightProjection * lightView;
//	return lightMatrice;
//}

//void RenderModule::DrawShadowPoint() {
//	float aspect = (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT;
//	float near_plane = 0.1f;
//	glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspect, near_plane, far_plane);
//
//
//	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
//	glBindFramebuffer(GL_FRAMEBUFFER, depthCubeMapFBO);  // Fbo unique par light
//	glClear(GL_DEPTH_BUFFER_BIT);
//
//
//	for (glm::vec3 lightPos : pointLightPositions) {
//		std::vector<glm::mat4> shadowTransforms;
//
//		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
//		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
//		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
//		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
//		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
//		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));
//
//
//		depthShaderCubeMap->Use();
//		depthShaderCubeMap->setFloat("far_plane", far_plane);
//
//		for (int i = 0; i < shadowTransforms.size(); i++) {
//			depthShaderCubeMap->setMatrix("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);
//		}
//
//		depthShaderCubeMap->setVec3("lightPos", lightPos);
//
//
//		cube->DrawObject(depthShaderCubeMap, true);
//		house->DrawObject(depthShaderCubeMap, true);
//	}
//
//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
//	glViewport(0, 0, Window::GetWidth(), Window::GetHeight());
//}

void RenderModule::DrawMirorCube() {
	reflectShader->Use();

	glm::mat4 model = glm::mat4(1.0f);
	float angle = 20.0f;
	model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
	reflectShader->setMatrix("model", model);


	glm::mat4 projection = glm::perspective(glm::radians(mainCamera->GetZoom()), (float)Window::GetWidth() / (float)Window::GetHeight(), 0.1f, 100.0f);
	reflectShader->setMatrix("view", mainCamera->GetViewMatrix());
	reflectShader->setMatrix("projection", projection);


	// oui c'est pas en screenspace
	reflectShader->setVec3("cameraPos", mainCamera->GetPos());

	glActiveTexture(GL_TEXTURE0);
	reflectShader->setInt("skybox", 0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);


	glBindVertexArray(reflectVAO);

	//glm::mat3 normalViewMatrix = glm::transpose(glm::inverse(glm::mat3(mainCamera->GetViewMatrix() * _model)));
	//reflectShader->setMatrix("normalViewMatrix", normalViewMatrix);

	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

void RenderModule::DrawLight(int indice){
	unsigned int lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);

	unsigned int VBO;  // Vertex Buffer Object
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesLightRec), verticesLightRec, GL_STATIC_DRAW);


	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// set the vertex attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	shaderLight->Use();

	//shaderLight->setVec3("objectColor", glm::vec3(1.0f, 0.5f, 0.31f));
	//shaderLight->setVec3("lightColor", glm:: vec3(1.0f, 1.0f, 1.0f));

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, pointLightPositions[indice]);
	model = glm::scale(model, glm::vec3(0.2f));

	shaderLight->setMatrix("model", model);


	shaderLight->setMatrix("view", mainCamera->GetViewMatrix());
	glm::mat4 projection = glm::perspective(glm::radians(mainCamera->GetZoom()), (float)Window::GetWidth() / (float)Window::GetHeight(), 0.1f, 100.0f);
	shaderLight->setMatrix("projection", projection);


	glBindVertexArray(lightVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);


	glDeleteVertexArrays(1, &lightVAO);
	glDeleteBuffers(1, &VBO);
	glBindVertexArray(0);
}

void RenderModule::DrawTextureOnScreen() {

	ppShader->Use();
	glBindVertexArray(quadVAO);

	glDisable(GL_DEPTH_TEST);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, finalTxtOutput);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, finalTxtColorOutput);
	glDrawArrays(GL_TRIANGLES, 0, 6);


	glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBlitFramebuffer(0, 0, Window::GetWidth(), Window::GetHeight(), 0, 0, Window::GetWidth(), Window::GetHeight(), GL_COLOR_BUFFER_BIT, GL_NEAREST);

	glBindVertexArray(0);
	glEnable(GL_DEPTH_TEST);
}

void RenderModule::InitQuadVao() {
	//Init fbo
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);


	//Init texture depth
	glGenTextures(1, &finalTxtOutput);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, finalTxtOutput);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, sample, GL_DEPTH_COMPONENT24, Window::GetWidth(), Window::GetHeight(), GL_TRUE);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, finalTxtOutput, 0);
	//

	//Init texture color
	glGenTextures(1, &finalTxtColorOutput);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, finalTxtColorOutput);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, sample, GL_RGB, Window::GetWidth(), Window::GetHeight(), GL_TRUE);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, finalTxtColorOutput, 0);
	//

	//Assert
	if (!glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
		std::cout << "L'Init du quadVao a echouer" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	//Init quadVAO
	glGenVertexArrays(1, &quadVAO);
	glBindVertexArray(quadVAO);

	glGenBuffers(1, &quadVBO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

	//Position
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//Texture
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
}

void RenderModule::InitSkyBox() {
	cubemapTexture = TextureClass::LoadCubeMapFromFile(faces);
	//Init quadVAO
	glGenVertexArrays(1, &skyboxVAO);
	glBindVertexArray(skyboxVAO);

	glGenBuffers(1, &skyboxVBO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);

	//Position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);


	glBindVertexArray(0);
}

void RenderModule::DrawSkyBox(glm::mat4 projectionMatrix) {
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_FALSE);

	skyboxShader->Use();

	skyboxShader->setMatrix("projection", projectionMatrix);
	skyboxShader->setMatrix("view", glm::mat4(glm::mat3(mainCamera->GetViewMatrix())));


	glBindVertexArray(skyboxVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glDrawArrays(GL_TRIANGLES, 0, 36);


	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LESS);

	glBindVertexArray(0);

}

void RenderModule::InitShadowMap() {
	glGenFramebuffers(1, &depthMapFBO);

	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH,SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);


	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);

	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Shadow Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderModule::InitCubeMap() {
	glGenFramebuffers(1, &depthCubeMapFBO);

	glGenTextures(1, &depthCubemap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
	for (unsigned int i = 0; i < 6; ++i)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);


	glBindFramebuffer(GL_FRAMEBUFFER, depthCubeMapFBO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);

	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "CubeMap Shadow Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

//void RenderModule::DrawShadowMap() {  // Bug sur la window si resize
//	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
//	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
//	glClear(GL_DEPTH_BUFFER_BIT);
//
//	depthShader->Use();
//	dirLight->UseShadow(depthShader);
//	//depthShader->setMatrix("view", mainCamera->GetViewMatrix());
//
//	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//	
//	//glm::rotate(_model, glm::radians(90.0f), glm::vec3(1, 0, 0))
//	cube->DrawObject(depthShader, true);
//	house->DrawObject(depthShader, true);
//
//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
//	glViewport(0, 0, Window::GetWidth(), Window::GetHeight());
//}

void RenderModule::Init() {
	Window* windowClass = Window::GetInstance();

	window = windowClass->GetWindow();
	if (window == nullptr) {
		std::cout << "Reference de la window impossible a recuperer" << std::endl;
		abort();
	}
	mainShader = std::make_shared<Shader>("TriangleOne/Shader/MainShader/BaseVertexShader.glsl", "TriangleOne/Shader/MainShader/BaseFragmentShader.glsl");
	depthShader = std::make_shared<Shader>("TriangleOne/Shader/LightShader/ShadowMapping/DepthMapVertex.glsl", "TriangleOne/Shader/LightShader/ShadowMapping/DepthMapFrag.glsl");

	shaderLight = new Shader("TriangleOne/Shader/LightShader/Light/LightVertexShader.glsl", "TriangleOne/Shader/LightShader/Light/LightFragShader.glsl");
	ppShader = new Shader("TriangleOne/Shader/PostProcessShader/PostProcessVertex.glsl", "TriangleOne/Shader/PostProcessShader/PostProcessFrag.glsl");
	skyboxShader = new Shader("TriangleOne/Shader/MiscShader/SkyBoxVertex.glsl", "TriangleOne/Shader/MiscShader/SkyBoxFrag.glsl");
	reflectShader = new Shader("TriangleOne/Shader/MiscShader/ReflexionVertex.glsl", "TriangleOne/Shader/MiscShader/ReflexionFrag.glsl");


	depthShaderCubeMap = new Shader("TriangleOne/Shader/LightShader/ShadowMapping/ShadowCubeVertex.glsl", "TriangleOne/Shader/LightShader/ShadowMapping/ShadowCubeFrag.glsl", "TriangleOne/Shader/LightShader/ShadowMapping/ShadowCubeGeometry.glsl");


	//Blending     //ya pas de blending mm avec cette ligne au cas ou 
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Face culling     //fonctionne bizzarement ( faudrait chek l'ordre de dessin des vertex)
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	//glFrontFace(GL_CCW);


	//MSAA
	glEnable(GL_MULTISAMPLE);

	glEnable(GL_DEPTH_TEST);

	//Correction gamma 
	//glEnable(GL_FRAMEBUFFER_SRGB);

	//Camera 
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	mainCamera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f));

	//pointLightPositions.push_back(glm::vec3(0.7f, 0.2f, 2.0f));
	//pointLightPositions.push_back(glm::vec3(2.3f, -3.3f, -4.0f));
	//pointLightPositions.push_back(glm::vec3(-4.0f, 2.0f, -12.0f));
	pointLightPositions.push_back(glm::vec3(1.0f, 2.0f, 0.0f));


	//house = new GameObject("Assets/ImpScene/autumn_house.glb");
	//cube = new Entity("Assets/ImpScene/cubeDebeguer.glb");
	//cube->SetPosition(glm::vec3(10.0f, 6.0f, 0.0f));
	//modelMesh2 = new Model("Assets/tryModel/backpacka.obj");


	renderSystem = std::make_shared<RenderSystem>();

	currentScene = std::make_shared<Scene>(renderSystem);

	std::shared_ptr<Entity> entity = currentScene->CreateNewEntity();
	std::shared_ptr<MeshComponent> meshAttachToEntity = currentScene->AddComponent<MeshComponent>(entity, "Assets/ImpScene/autumn_house.glb", mainShader);



	if (glfwGetCurrentContext() == NULL) {
		std::cout << "FATAL: Tentative de création de Mesh sans contexte OpenGL !" << std::endl;
	}

	glm::vec3 ambient = glm::vec3(0.3f, 0.3f, 0.3f);
	glm::vec3 diffuse = glm::vec3(3.0f, 3.0f, 3.0f);
	glm::vec3 specular = glm::vec3(0.0f, 0.0f, 0.0f);

	glm::vec3 worldLightDir = glm::vec3(-2.0f, 4.0f, -1.0f);

	std::shared_ptr<Entity> entityLight = currentScene->CreateNewEntity();
	std::shared_ptr<DirLight> dirLight = currentScene->AddComponent<DirLight>(entityLight, glm::vec3(0),ambient, diffuse, specular, worldLightDir, depthShader);

	//	DirLight(glm::vec3 _position, glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular, glm::vec3 direction, std::shared_ptr<Shader> _depthShader);


	//dirLight = new Light(glm::vec3(0), worldLightDir, ambient, diffuse, specular);

	InitQuadVao();

	InitSkyBox();

	InitShadowMap();

	InitCubeMap();

}

void RenderModule::Render()
 {
	//Shadow

	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//DrawMirorCube();
	for (int i = 0; i < pointLightPositions.size(); i++)
		DrawLight(i);

	//mainShader->Use();
	//mainShader->setFloat("far_plane", far_plane);  
	//mainShader->setFloat("material.shininess", 32.0f);
	//mainShader->setVec3("viewPos", mainCamera->GetPos());

	//for (int i = 0; i < pointLightPositions.size(); i++)
	//	FactoPointLight(mainShader, i);


	//FactoSpotLight(mainShader, 0);


	glm::mat4 projection = glm::perspective(glm::radians(mainCamera->GetZoom()), (float)Window::GetWidth() / (float)Window::GetHeight(), 0.1f, 100.0f);
	glm::mat4 model = glm::rotate(_model, glm::radians(90.0f), glm::vec3(1, 0, 0));

	mainShader->Use(); 
	mainShader->setMatrix("model", _model);
	mainShader->setMatrix("view", mainCamera->GetViewMatrix());
	mainShader->setMatrix("projection", projection);


	//mainShader->setInt("shadowMap", 15);
	//glActiveTexture(GL_TEXTURE15);
	//glBindTexture(GL_TEXTURE_2D, depthMap);

	//mainShader->setInt("shadowCubeMap", 2); ////////////////////////////////////////////////////////////////////////////  ID DEJA ATTRIBUER, BUG POTENTIELLE
	//glActiveTexture(GL_TEXTURE2);
	//glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);

	//
	//house->DrawObject(mainShader, false);
	//cube->DrawObject(mainShader, false);


	currentScene->RenderScene();


	//DrawSkyBox(projection);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	DrawTextureOnScreen();

	// A deplacer dans un input manager
	mainCamera->ProcessInput(window);
	
	glfwSwapBuffers(window);
	glfwPollEvents();
}

void RenderModule::Shutdown() {
	std::cout << "Shuting down RenderModule" << std::endl;
}
