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
	Shader::CreateDefaultWhiteTexture();
	mainShader = std::make_shared<Shader>("TriangleOne/Shader/MainShader/BaseVertexShader.glsl", "TriangleOne/Shader/MainShader/BaseFragmentShader.glsl");
	depthShader = std::make_shared<Shader>("TriangleOne/Shader/LightShader/ShadowMapping/DepthMapVertex.glsl", "TriangleOne/Shader/LightShader/ShadowMapping/DepthMapFrag.glsl");
	depthShaderCubeMap = std::make_shared<Shader>("TriangleOne/Shader/LightShader/ShadowMapping/ShadowCubeVertex.glsl", "TriangleOne/Shader/LightShader/ShadowMapping/ShadowCubeFrag.glsl", "TriangleOne/Shader/LightShader/ShadowMapping/ShadowCubeGeometry.glsl");

	shaderLight = new Shader("TriangleOne/Shader/LightShader/Light/LightVertexShader.glsl", "TriangleOne/Shader/LightShader/Light/LightFragShader.glsl");
	ppShader = new Shader("TriangleOne/Shader/PostProcessShader/PostProcessVertex.glsl", "TriangleOne/Shader/PostProcessShader/PostProcessFrag.glsl");
	skyboxShader = new Shader("TriangleOne/Shader/MiscShader/SkyBoxVertex.glsl", "TriangleOne/Shader/MiscShader/SkyBoxFrag.glsl");
	reflectShader = new Shader("TriangleOne/Shader/MiscShader/ReflexionVertex.glsl", "TriangleOne/Shader/MiscShader/ReflexionFrag.glsl");



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


	pointLightPositions.push_back(glm::vec3(1.0f, 2.0f, 0.0f));


	//house = new GameObject("Assets/ImpScene/autumn_house.glb");
	//cube = new Entity("Assets/ImpScene/cubeDebeguer.glb");
	//cube->SetPosition(glm::vec3(10.0f, 6.0f, 0.0f));
	//modelMesh2 = new Model("Assets/tryModel/backpacka.obj");


	renderSystem = new RenderSystem(&framebuffer, mainCamera);

	currentScene = new Scene();
	//Maison
	Entity* entity = currentScene->CreateNewEntity();
	entity->AddComponent<Transform>();
	entity->AddComponent<MeshComponent>("Assets/ImpScene/autumn_house.glb", mainShader);
	//std::shared_ptr<MeshComponent> meshAttachToEntity = currentScene->AddComponent<MeshComponent>(entity, "Assets/ImpScene/autumn_house.glb", mainShader);



	glm::vec3 ambient = glm::vec3(0.3f, 0.3f, 0.3f);
	glm::vec3 diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 specular = glm::vec3(0.0f, 0.0f, 0.0f);

	float intensity = 3.0f;

	glm::vec3 worldLightDir = glm::vec3(-2.0f, 4.0f, -1.0f);

	//DirLight   	//	DirLight(glm::vec3 _position, glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular, glm::vec3 _direction, std::shared_ptr<Shader> _depthShader)
	Entity* entityLight = currentScene->CreateNewEntity();
	entityLight->AddComponent<Transform>();
	entityLight->AddComponent<DirLight>(ambient, diffuse, specular, worldLightDir, depthShader, intensity);

	float pointLightrange= 5.0f;


	//PointLight
	Entity* entityPointLight = currentScene->CreateNewEntity();
	entityPointLight->AddComponent<Transform>()->position = glm::vec3(0.0f, -0.1f, 0.0f);

	entityPointLight->AddComponent<PointLight>(ambient, diffuse, specular, pointLightrange, depthShaderCubeMap, 5.0f);

	Entity* cubeTest = currentScene->CreateNewEntity();
	cubeTest->AddComponent<Transform>()->position = glm::vec3(1.0f, 0.0f, 0.0f);
	cubeTest->AddComponent<MeshComponent>("Assets/ImpScene/BasicCube.glb", mainShader);



	//	PointLight(glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular, float _constant, float _linear, float _quadratique, std::shared_ptr<Shader> _depthShaderCubeMap) 


	//dirLight = new Light(glm::vec3(0), worldLightDir, ambient, diffuse, specular);


	InitQuadVao();
	InitSkyBox();
}

void RenderModule::Render()
 {
	//Shadow

	for (int i = 0; i < pointLightPositions.size(); i++)
		DrawLight(i);


	glm::mat4 projection = glm::perspective(glm::radians(mainCamera->GetZoom()), (float)Window::GetWidth() / (float)Window::GetHeight(), mainCamera->GetNearPlane(), mainCamera->GetFarPlane());

	mainShader->Use(); 
	mainShader->setMatrix("model", _model);
	mainShader->setMatrix("view", mainCamera->GetViewMatrix());
	mainShader->setMatrix("projection", projection);

	mainShader->setVec3("viewPos", mainCamera->GetPos());


	renderSystem->RenderScene(currentScene, projection);


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
