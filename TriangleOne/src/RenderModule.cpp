#include <RenderModule.h>

float vertices[] = {
	0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f, // bottom right
	-0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f, // bottom left
	0.0f, 0.5f, 0.0f,   0.0f, 0.0f, 1.0f // top
};

float verticesRec[] = {
0.5f, 0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0,  // top right
0.5f, -0.5f, 0.0f,    0.0f, 1.0f, 0.0f,  1.0f, 0.0f,   // bottom right
-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,  // bottom left
-0.5f, 0.5f, 0.0f,    0.0f, 1.0f, 0.0f,  0.0f, 1.0f   // top left
};
unsigned int indices[] = { // note that we start from 0!
0, 1, 3, // first triangle
1, 2, 3 // second triangle
};

float texCoords[] = {
0.0f, 0.0f, // lower-left corner
1.0f, 0.0f, // lower-right corner
0.5f, 1.0f // top-center corner
};

RenderModule* RenderModule::instance = nullptr;

RenderModule::RenderModule() {
	instance = this;
}

RenderModule* RenderModule::GetInstance() {
	return instance;
}


void RenderModule::DrawTriangle() {
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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//Color
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);


	shader->Use();


	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3);
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
}

void RenderModule::Init() {
	Window* windowClass = Window::GetInstance();

	window = windowClass->GetWindow();
	if (window == nullptr) {
		std::cout << "Reference de la window imposible a recuperer" << std::endl;
		abort();
	}
	shader = new Shader("TriangleOne/Shader/BaseVertexShader.glsl", "TriangleOne/Shader/BaseFragmentShader.glsl");
	texture = new Texture("Assets/woodPng.png");
}

void RenderModule::Render() {
	glClear(GL_COLOR_BUFFER_BIT);

	//DrawTriangle();
	DrawRectangle();

	glfwSwapBuffers(window);
	glfwPollEvents();
}

void RenderModule::Shutdown() {
	std::cout << "Shuting down RenderModule" << std::endl;
}
