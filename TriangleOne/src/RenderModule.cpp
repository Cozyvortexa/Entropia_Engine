#include <RenderModule.h>


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
	glm::vec3 viewPosition = mainCamera->GetViewMatrix() * glm::vec4(pointLightPositions[i], 1.0f);
	lightShader->setVec3("pointLights[" + std::to_string(i) + "].viewPosition", viewPosition);

	lightShader->setVec3("pointLights[" + std::to_string(i) + "].ambient", glm::vec3(0.2f, 0.2f, 0.2f));
	lightShader->setVec3("pointLights[" + std::to_string(i) + "].diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
	lightShader->setVec3("pointLights[" + std::to_string(i) + "].specular", glm::vec3(1.0f, 1.0f, 1.0f));

	lightShader->setFloat("pointLights[" + std::to_string(i) + "].constant", 1.0f);
	lightShader->setFloat("pointLights[" + std::to_string(i) + "].linear", 0.09f);
	lightShader->setFloat("pointLights[" + std::to_string(i) + "].quadratic", 0.032f);
}

void RenderModule::FactoDirLight(Shader* lightShader,glm::vec3 worldLightPos) {
	lightShader->setVec3("dirLight.direction", glm::mat3(mainCamera->GetViewMatrix()) * worldLightPos);
	lightShader->setVec3("dirLight.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
	lightShader->setVec3("dirLight.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
	lightShader->setVec3("dirLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
}

void RenderModule::FactoSpotLight(Shader* lightShader, int i) {
	lightShader->setVec3("spotLight.viewPosition", glm::vec3(0, 0, 0));

	lightShader->setVec3("spotLight.ambient", glm::vec3(0.5f, 0.5f, 0.5f));
	lightShader->setVec3("spotLight.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
	lightShader->setVec3("spotLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));

	lightShader->setFloat("spotLight.constant", 1.0f);
	lightShader->setFloat("spotLight.linear", 0.09f);
	lightShader->setFloat("spotLight.quadratic", 0.032f);

	lightShader->setVec3("spotLight.direction", mainCamera->GetFront());
	lightShader->setFloat("spotLight.cutOff", glm::cos(glm::radians(17.5f)));
	lightShader->setFloat("spotLight.outerCutOff", glm::cos(glm::radians(25.0f)));
}

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
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Basic Fortnite emote.txt" << std::endl;
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

void RenderModule::Init() {
	Window* windowClass = Window::GetInstance();

	window = windowClass->GetWindow();
	if (window == nullptr) {
		std::cout << "Reference de la window imposible a recuperer" << std::endl;
		abort();
	}
	shader = new Shader("TriangleOne/Shader/BaseVertexShader.glsl", "TriangleOne/Shader/BaseFragmentShader.glsl");
	shaderLight = new Shader("TriangleOne/Shader/LightVertexShader.glsl", "TriangleOne/Shader/LightFragShader.glsl");
	ppShader = new Shader("TriangleOne/Shader/PostProcessVertex.glsl", "TriangleOne/Shader/PostProcessFrag.glsl");
	skyboxShader = new Shader("TriangleOne/Shader/SkyBoxVertex.glsl", "TriangleOne/Shader/SkyBoxFrag.glsl");
	reflectShader = new Shader("TriangleOne/Shader/ReflexionVertex.glsl", "TriangleOne/Shader/ReflexionFrag.glsl");


	//Blending     //ya pas de blending mm avec c'est ligne au cas ou 
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Face culling     //fonctionne bizzarement ( faudrait chek l'ordre de dessin des vertex)
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	//glFrontFace(GL_CW);


	//MSAA
	glEnable(GL_MULTISAMPLE);

	//Camera 
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	mainCamera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f));

	lightPos = glm::vec3(1.2f, 1.0f, 2.0f);

	pointLightPositions.push_back(glm::vec3(0.7f, 0.2f, 2.0f));
	pointLightPositions.push_back(glm::vec3(2.3f, -3.3f, -4.0f));
	pointLightPositions.push_back(glm::vec3(-4.0f, 2.0f, -12.0f));
	pointLightPositions.push_back(glm::vec3(0.0f, 0.0f, -3.0f));


	modelMesh = new Model("Assets/ImpScene/house_on_the_hill.glb");
	//modelMesh = new Model("Assets/tryModel/backpacka.obj");


	InitQuadVao();

	InitSkyBox();


	glGenVertexArrays(1, &reflectVAO);
	glBindVertexArray(reflectVAO);


	glGenBuffers(1, &reflectVBO);
	glBindBuffer(GL_ARRAY_BUFFER, reflectVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesNormal), verticesNormal, GL_STATIC_DRAW);


	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
}

void RenderModule::Render()
 {
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	//DrawMirorCube();

	for (int i = 0; i < pointLightPositions.size();i++)
		DrawLight(i);
	//DrawCubeAffectedByFlashLight();

	shader->Use();

	shader->setFloat("material.shininess", 32.0f);

	for (int i = 0; i < pointLightPositions.size(); i++)
		FactoPointLight(shader, i);

	glm::vec3 worldLightDir = glm::vec3(-0.2f, -1.0f, -0.3f);
	FactoDirLight(shader, worldLightDir);
	FactoSpotLight(shader, 0);

	glm::mat4 projection = glm::perspective(glm::radians(mainCamera->GetZoom()), (float)Window::GetWidth() / (float)Window::GetHeight(), 0.1f, 100.0f);
	shader->setMatrix("model", _model);
	shader->setMatrix("view", mainCamera->GetViewMatrix());
	shader->setMatrix("projection", projection);
	//


	modelMesh->Draw(shader);
	DrawSkyBox(projection);
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
