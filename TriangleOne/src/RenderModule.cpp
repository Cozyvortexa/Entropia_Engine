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
	// positions // normals // texture coords
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, -1.0f,  0.0f, 0.0f,
	0.5f, -0.5f, -0.5f,  0.0f, 0.0f, -1.0f,  1.0f, 0.0f,
	0.5f, 0.5f, -0.5f,  0.0f, 0.0f, -1.0f,  1.0f, 1.0f,
	0.5f, 0.5f, -0.5f,  0.0f, 0.0f, -1.0f,  1.0f, 1.0f,
	-0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, -1.0f,  0.0f, 0.0f,

	-0.5f, -0.5f, 0.5f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f,
	0.5f, -0.5f, 0.5f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f,
	0.5f, 0.5f, 0.5f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f,
	0.5f, 0.5f, 0.5f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f,
	-0.5f, 0.5f, 0.5f,  0.0f, 0.0f, 1.0f,  0.0f, 1.0f,
	-0.5f, -0.5f, 0.5f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f,

	-0.5f, 0.5f, 0.5f,  -1.0f, 0.0f, 0.0f,  1.0f, 0.0f,
	-0.5f, 0.5f, -0.5f,  -1.0f, 0.0f, 0.0f,  1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  -1.0f, 0.0f, 0.0f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  -1.0f, 0.0f, 0.0f,  0.0f, 1.0f,
	-0.5f, -0.5f, 0.5f,  -1.0f, 0.0f, 0.0f,  0.0f, 0.0f,
	-0.5f, 0.5f, 0.5f,  -1.0f, 0.0f, 0.0f,  1.0f, 0.0f,

	0.5f, 0.5f, 0.5f,  1.0f, 0.0f, 0.0f,  1.0f, 0.0f,
	0.5f, 0.5f, -0.5f,  1.0f, 0.0f, 0.0f,  1.0f, 1.0f,
	0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f,
	0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f,
	0.5f, -0.5f, 0.5f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f,
	0.5f, 0.5f, 0.5f,  1.0f, 0.0f, 0.0f,  1.0f, 0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f,
	0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f,
	0.5f, -0.5f, 0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f,
	0.5f, -0.5f, 0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f,
	-0.5f, -0.5f, 0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f,

	-0.5f, 0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f,
	0.5f, 0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f,
	0.5f, 0.5f, 0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f,
	0.5f, 0.5f, 0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f,
	-0.5f, 0.5f, 0.5f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
	-0.5f, 0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f
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

void RenderModule::DrawCubeAffectedByFlashLight() {
	unsigned int VAO;  // Vertex Array Object
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//Vertex input
	unsigned int VBO;  // Vertex Buffer Object
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesNormal), verticesNormal, GL_STATIC_DRAW);

	/*
	GL_STREAM_DRAW: the data is set only once and used by the GPU at most a few times.
	GL_STATIC_DRAW: the data is set only once and used many times.
	GL_DYNAMIC_DRAW: the data is changed a lot and used many times.
	*/

	//Position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//Normal
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//PosTexture
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);


	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture->getTexture());

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textureSpecular->getTexture());


	shader->Use();

	//Materiaux
	//shader->setVec3("material.ambient", glm::vec3(0.19225f, 0.19225f, 0.19225f));
	//shader->setVec3("material.diffuse", glm::vec3(0.50754f, 0.50754f, 0.50754f));
	shader->setInt("material.diffuse", 0);
	shader->setInt("material.specular", 1);
	shader->setFloat("material.shininess", 32.0f);

	//Light
	//shader->setVec3("light.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
	//shader->setVec3("light.diffuse", glm::vec3(0.5f, 0.5f, 0.5f)); // darkened
	//shader->setVec3("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));

	//shader->setFloat("light.constant", 1.0f);
	//shader->setFloat("light.linear", 0.09f);
	//shader->setFloat("light.quadratic", 0.032f);

	//Point Lights
	for (int i = 0; i < pointLightPositions.size(); i++)
		FactoPointLight(shader, i);

	//Directional Light
	glm::vec3 worldLightDir = glm::vec3(-0.2f, -1.0f, -0.3f);
	FactoDirLight(shader, worldLightDir);

	FactoSpotLight(shader, 0);


	//glm::vec3 lightPosView = glm::vec3(mainCamera->GetViewMatrix() * glm::vec4(lightPos, 1.0));  // on passe les coordonée de la lumiere (Pos monde) en pos view
	//shader->setVec3("light.viewPosition", lightPosView);

	// coordonée de lumiere en viewSpace
	//glm::mat3 normalViewMatrix = glm::transpose(glm::inverse(glm::mat3(mainCamera->GetViewMatrix() * _model)));
	//shader->setMatrix("normalViewMatrix", normalViewMatrix);


	// les trois matrices
	glm::mat4 projection = glm::perspective(glm::radians(mainCamera->GetZoom()), (float)Window::GetWidth() / (float)Window::GetHeight(), 0.1f, 100.0f);
	shader->setMatrix("model", _model);
	shader->setMatrix("view", mainCamera->GetViewMatrix());
	shader->setMatrix("projection", projection);


	glBindVertexArray(VAO);
	for (unsigned int i = 0; i < 10; i++)
	{
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, cubePositions[i]);
		float angle = 20.0f * i;
		model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
		shader->setMatrix("model", model);

		// coordonée de lumiere en viewSpace
		glm::mat3 normalViewMatrix = glm::transpose(glm::inverse(glm::mat3(mainCamera->GetViewMatrix() * model)));
		shader->setMatrix("normalViewMatrix", normalViewMatrix);

		glDrawArrays(GL_TRIANGLES, 0, 36);
	}


	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
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
}

void RenderModule::DrawTextureOnScreen() {

	ppShader->Use();
	glBindVertexArray(quadVAO);

	glDisable(GL_DEPTH_TEST);
	glBindTexture(GL_TEXTURE_2D, finalTxtOutput);
	glBindTexture(GL_TEXTURE_2D, finalTxtColorOutput);
	glDrawArrays(GL_TRIANGLES, 0, 6);


	//glDeleteVertexArrays(1, &quadVAO);
	//glDeleteBuffers(1, &quadVBO);
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

	texture = new TextureClass("Assets/image.png");
	textureSpecular = new TextureClass("Assets/imageSpecular.png");

	//Depth testing
	//glEnable(GL_DEPTH_TEST);

	//Blending     //ya pas de blending mm avec c'est ligne au cas ou 
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Face culling     //fonctionne bizzarement ( faudrait chek l'ordre de dessin des vertex)
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	//glFrontFace(GL_CW);


	//MSAA
	//glEnable(GL_MULTISAMPLE);

	//Camera 
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	mainCamera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f));

	lightPos = glm::vec3(1.2f, 1.0f, 2.0f);

	pointLightPositions.push_back(glm::vec3(0.7f, 0.2f, 2.0f));
	pointLightPositions.push_back(glm::vec3(2.3f, -3.3f, -4.0f));
	pointLightPositions.push_back(glm::vec3(-4.0f, 2.0f, -12.0f));
	pointLightPositions.push_back(glm::vec3(0.0f, 0.0f, -3.0f));


	modelMesh = new Model("Assets/tryModel/backpacka.obj");
	//modelMesh = new Model("Assets/doubleTry/red-renault-carwreck.fbx");


	//Init fbo
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	//Init texture depth
	glGenTextures(1, &finalTxtOutput);
	glBindTexture(GL_TEXTURE_2D, finalTxtOutput);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, Window::GetWidth(), Window::GetHeight(), 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);


	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);


	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, finalTxtOutput, 0);


	//Init texture color
	glGenTextures(1, &finalTxtColorOutput);
	glBindTexture(GL_TEXTURE_2D, finalTxtColorOutput);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Window::GetWidth(), Window::GetHeight(), 0, GL_RGB, GL_FLOAT, NULL);


	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);


	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, finalTxtColorOutput, 0);






	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Basic Fortnite emote.txt" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);



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

void RenderModule::Render()
 {
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	for (int i = 0; i < pointLightPositions.size();i++)
		DrawLight(i);
	//DrawCubeAffectedByFlashLight();

	shader->Use();

	//Temp
	shader->setFloat("material.shininess", 32.0f);
	//Light
	for (int i = 0; i < pointLightPositions.size(); i++)
		FactoPointLight(shader, i);
	glm::vec3 worldLightDir = glm::vec3(-0.2f, -1.0f, -0.3f);

	FactoDirLight(shader, worldLightDir);

	FactoSpotLight(shader, 0);
	//


	glm::mat4 projection = glm::perspective(glm::radians(mainCamera->GetZoom()), (float)Window::GetWidth() / (float)Window::GetHeight(), 0.1f, 100.0f);
	shader->setMatrix("model", _model);
	shader->setMatrix("view", mainCamera->GetViewMatrix());
	shader->setMatrix("projection", projection);
	//


	modelMesh->Draw(shader);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT);


	DrawTextureOnScreen();



	mainCamera->ProcessInput(window);
	

	glfwSwapBuffers(window);
	glfwPollEvents();
}

void RenderModule::Shutdown() {
	std::cout << "Shuting down RenderModule" << std::endl;
}
