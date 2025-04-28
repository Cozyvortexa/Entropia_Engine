#include <Render.h>

float vertices[] = {
	-0.5f, -0.5f, 0.0f,
	0.5f, -0.5f, 0.0f,
	0.0f, 0.5f, 0.0f
};

std::string ReadFileToString(const std::string& filePath) {
	std::ifstream file(filePath);
	if (!file.is_open()) {
		std::cerr << "Erreur : impossible d'ouvrir le fichier " << filePath << std::endl;
		return "";
	}

	std::stringstream buffer;
	buffer << file.rdbuf();
	return buffer.str();
}

void Render::CreateShader(std::string path, int methode) {  // methode a transformer en enum
	std::string vertexCode = ReadFileToString(path);
	const char* codeCStr = vertexCode.c_str();

	unsigned int shader;
	if (methode == 1) {
		shader = glCreateShader(GL_FRAGMENT_SHADER);
	}
	else {
		shader = glCreateShader(GL_VERTEX_SHADER);
	}

	glShaderSource(shader, 1, &codeCStr, NULL);
	glCompileShader(shader);

	//Verification du shader
	int success;
	char infoLog[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		return;
	}
	shaderListe.push_back(shader);
}

void Render::CreateShaderProg() {
	shaderProgram = glCreateProgram();
	for (auto& shader : shaderListe) {
		glAttachShader(shaderProgram, shader);
	}
	glLinkProgram(shaderProgram);

	//Verif
	int success;
	char infoLog[512];
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROG::CREATION_FAILED\n" << infoLog << std::endl;
		return;
	}
	//

	for (auto& shader : shaderListe) {  // oui en deux temps 
		glDeleteShader(shader);
	}
	shaderListe.clear();

}

void Render::DrawTriangle() {
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//Vertex input
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	/*
	GL_STREAM_DRAW: the data is set only once and used by the GPU at most a few times.
	GL_STATIC_DRAW: the data is set only once and used many times.
	GL_DYNAMIC_DRAW: the data is changed a lot and used many times.
	*/



	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);



	//Vertex shader
	CreateShader("TriangleOne/Shader/BaseVertexShader.glsl", 0);  // simplifier le chemin

	CreateShader("TriangleOne/Shader/BaseFragmentShader.glsl", 1);

	CreateShaderProg();

	glUseProgram(shaderProgram);
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3);
}

void Render::Framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void Render::ProcessInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}


void Render::Init() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(WIDHT, HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		abort();
	}


	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		abort();
	}
	glfwSetFramebufferSizeCallback(window, Framebuffer_size_callback); // Pour adapter le viewport si la fenetre est resize pendant le court du programme 
	glViewport(0, 0, WIDHT, HEIGHT);

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
}

void Render::Update() {

	//while (!glfwWindowShouldClose(window))
	//{

	//}

	ProcessInput(window);  // gere les inputs 

	glClear(GL_COLOR_BUFFER_BIT);

	DrawTriangle();


	glfwSwapBuffers(window);
	glfwPollEvents();
}

void Render::Shutdown() {
	glfwTerminate();
}
