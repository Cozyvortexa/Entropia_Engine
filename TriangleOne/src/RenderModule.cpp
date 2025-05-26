#include <RenderModule.h>

float vertices[] = {
-0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
-0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
-0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

-0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
-0.5f, 0.5f, 0.5f, 0.0f, 1.0f,
-0.5f, -0.5f, 0.5f, 0.0f, 0.0f,

-0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
-0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
-0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
-0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
-0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
-0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

-0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
-0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
-0.5f, -0.5f, -0.5f, 0.0f, 1.0f,

-0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
-0.5f, 0.5f, 0.5f, 0.0f, 0.0f,
-0.5f, 0.5f, -0.5f, 0.0f, 1.0f
};

float verticesRec[] = {
//Pos				//Color				//Tex
0.5f, 0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0,  // top right
0.5f, -0.5f, 0.0f,    0.0f, 1.0f, 0.0f,  1.0f, 0.0f,   // bottom right
-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,  // bottom left
-0.5f, 0.5f, 0.0f,    0.0f, 1.0f, 0.0f,  0.0f, 1.0f   // top left
};

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

unsigned int indices[] = { // note that we start from 0!
0, 1, 3, // first triangle
1, 2, 3 // second triangle
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
	lightShader->setVec3("spotLight.viewPosition", glm::vec3(0.0f, 0.0f, 0.0f));

	lightShader->setVec3("spotLight.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
	lightShader->setVec3("spotLight.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
	lightShader->setVec3("spotLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));

	lightShader->setFloat("spotLight.constant", 1.0f);
	lightShader->setFloat("spotLight.linear", 0.09f);
	lightShader->setFloat("spotLight.quadratic", 0.032f);

	lightShader->setVec3("spotLight.direction", mainCamera->GetFront());
	lightShader->setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
	lightShader->setFloat("spotLight.outerCutOff", glm::cos(glm::radians(17.5f)));
}

void RenderModule::DrawMultipleCube() {
	unsigned int VAO;  // Vertex Array Object
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//Vertex input
	unsigned int VBO;  // Vertex Buffer Object
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	/*
	GL_STREAM_DRAW: the data is set only once and used by the GPU at most a few times.
	GL_STATIC_DRAW: the data is set only once and used many times.
	GL_DYNAMIC_DRAW: the data is changed a lot and used many times.
	*/

	//Position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//Texture
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindTexture(GL_TEXTURE_2D, texture->getTexture());
	shader->Use();

	glBindVertexArray(VAO);
	for (unsigned int i = 0; i < 10; i++)
	{
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, cubePositions[i]);
		float angle = 20.0f * i;
		model = glm::rotate(model, (float)glfwGetTime() * glm::radians(20.0f + i * 10.0f), glm::vec3(0.5f, 1.0f, 0.0f));

		shader->setMatrix("model", model);

		shader->setMatrix("view", mainCamera->GetViewMatrix());

		glm::mat4 projection = glm::perspective(glm::radians(mainCamera->GetZoom()), (float)Window::GetWidth() / (float)Window::GetHeight(), 0.1f, 100.0f);
		shader->setMatrix("projection", projection);

		glDrawArrays(GL_TRIANGLES, 0, 36);

	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}

void RenderModule::DrawRectangle() {
	unsigned int VAO;  // Vertex Array Object
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	unsigned int EBO;  // Element Buffer Object
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	unsigned int VBO;  // Vertex Buffer Object
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesRec), verticesRec, GL_STATIC_DRAW);

	/*
	GL_STREAM_DRAW: the data is set only once and used by the GPU at most a few times.
	GL_STATIC_DRAW: the data is set only once and used many times.
	GL_DYNAMIC_DRAW: the data is changed a lot and used many times.
	*/

	////glm::mat4 trans = glm::mat4(1.0f);
	////trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(1.0f, 0.0f, 0.0f));

	////unsigned int transformLoc = glGetUniformLocation(shader->shaderID, "transform");
	////glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

	//Position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//Color
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//PosTexture
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindTexture(GL_TEXTURE_2D, texture->getTexture());
	shader->Use();

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	glBindVertexArray(VAO);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}

void RenderModule::DrawCubeAffectedByLight() {
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
	shader->setVec3("light.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
	shader->setVec3("light.diffuse", glm::vec3(0.5f, 0.5f, 0.5f)); // darkened
	shader->setVec3("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));

	shader->setFloat("light.constant", 1.0f);
	shader->setFloat("light.linear", 0.09f);
	shader->setFloat("light.quadratic", 0.032f);

	glm::vec3 lightPosView = glm::vec3(mainCamera->GetViewMatrix() * glm::vec4(lightPos, 1.0));  // on passe les coordonée de la lumiere (Pos monde) en pos view
	shader->setVec3("light.viewPosition", lightPosView);

	// coordonée de lumiere en viewSpace
	//glm::mat3 normalViewMatrix = glm::transpose(glm::inverse(glm::mat3(mainCamera->GetViewMatrix() * _model)));
	//shader->setMatrix("normalViewMatrix", normalViewMatrix);


	// les trois matrices
	glm::mat4 projection = glm::perspective(glm::radians(mainCamera->GetZoom()), (float)Window::GetWidth() / (float)Window::GetHeight(), 0.1f, 100.0f);
	shader->setMatrix("model", _model);
	shader->setMatrix("view", mainCamera->GetViewMatrix());
	shader->setMatrix("projection", projection);

	//glm::vec3 worldLightDir = glm::vec3(-0.2f, -1.0f, -0.3f);
	//shader->setVec3("light.direction", glm::mat3(mainCamera->GetViewMatrix()) * worldLightDir);

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

void RenderModule::Init() {
	Window* windowClass = Window::GetInstance();

	window = windowClass->GetWindow();
	if (window == nullptr) {
		std::cout << "Reference de la window imposible a recuperer" << std::endl;
		abort();
	}
	shader = new Shader("TriangleOne/Shader/BaseVertexShader.glsl", "TriangleOne/Shader/BaseFragmentShader.glsl");
	shaderLight = new Shader("TriangleOne/Shader/LightVertexShader.glsl", "TriangleOne/Shader/LightFragShader.glsl");

	texture = new Texture("Assets/image.png");
	textureSpecular = new Texture("Assets/imageSpecular.png");

	glEnable(GL_DEPTH_TEST);

	//Camera 
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	mainCamera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f));

	lightPos = glm::vec3(1.2f, 1.0f, 2.0f);

	pointLightPositions.push_back(glm::vec3(0.7f, 0.2f, 2.0f));
	pointLightPositions.push_back(glm::vec3(2.3f, -3.3f, -4.0f));
	pointLightPositions.push_back(glm::vec3(-4.0f, 2.0f, -12.0f));
	pointLightPositions.push_back(glm::vec3(0.0f, 0.0f, -3.0f));
}

void RenderModule::Render()
 {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//DrawMultipleCube();
	for (int i = 0; i < pointLightPositions.size();i++)
		DrawLight(i);
	DrawCubeAffectedByFlashLight();
	//DrawCubeAffectedByLight();
	//DrawRectangle();

	mainCamera->ProcessInput(window);
	

	glfwSwapBuffers(window);
	glfwPollEvents();
}

void RenderModule::Shutdown() {
	std::cout << "Shuting down RenderModule" << std::endl;
}
