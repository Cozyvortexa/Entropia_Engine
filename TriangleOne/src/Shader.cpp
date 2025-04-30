#include "Shader.h"

Shader::Shader(const char* vertexPath, const char* fragmentPath) {

	std::string vertexCode = ReadFile(vertexPath);  //Recuperation du vertexShader
	const char* vShaderCode = vertexCode.c_str();

	std::string fragmentCode = ReadFile(fragmentPath);  //Recuperation du fragmentShader 
	const char* fShaderCode = fragmentCode.c_str();

	unsigned int shaderVertex = glCreateShader(GL_VERTEX_SHADER);  // Création du vertex shader
	glShaderSource(shaderVertex, 1, &vShaderCode, NULL);
	glCompileShader(shaderVertex);

	unsigned int shaderFrag = glCreateShader(GL_FRAGMENT_SHADER);  // Création du vertex fragment
	glShaderSource(shaderFrag, 1, &fShaderCode, NULL);
	glCompileShader(shaderFrag);


	if (AssertShader(shaderVertex) || AssertShader(shaderFrag))  // On vérifie que les deux shaders ont etait correctement creer
		throw std::invalid_argument("Chemin incorrect entrer dans le constructeur");

	shaderID = glCreateProgram();

	glAttachShader(shaderID, shaderVertex);  //link des deux shader
	glAttachShader(shaderID, shaderFrag);

	glLinkProgram(shaderID);


	//Assert du shaderProgram
	int success;
	char infoLog[512];
	glGetProgramiv(shaderID, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderID, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROG::CREATION_FAILED\n" << infoLog << std::endl;
		throw  std::invalid_argument("ShaderProg");
	}

	glDeleteShader(shaderVertex);
	glDeleteShader(shaderFrag);
}

void Shader::Use()
{
	glUseProgram(shaderID);
}

std::string Shader::ReadFile(const char* shaderPath) {
	std::ifstream file(shaderPath);
	if (!file.is_open()) {
		std::cerr << "Erreur : impossible d'ouvrir le fichier " << std::endl;
		return "";
	}

	std::stringstream buffer;
	buffer << file.rdbuf();
	return buffer.str();
}

int Shader::AssertShader(unsigned int& shader) {

	int success;
	char infoLog[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
		return 1;
	}
	return 0;
}