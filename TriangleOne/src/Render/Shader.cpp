#include "Render/Shader.h"

Shader::Shader(const char* vertexPath, const char* fragmentPath) {
	std::filesystem::path vertex_FullPath = SHADER_DIR / vertexPath;
	std::filesystem::path fragment_FullPath = SHADER_DIR / fragmentPath;


	std::string vertexCode = ReadFile(vertex_FullPath.string().c_str());  //Recuperation du vertexShader
	const char* vShaderCode = vertexCode.c_str();

	std::string fragmentCode = ReadFile(fragment_FullPath.string().c_str());  //Recuperation du fragmentShader 
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
		std::cout << "VertexPath :" << vertex_FullPath.string() << std::endl;
		std::cout << "FragPath :" << fragment_FullPath.string() << std::endl;
		throw  std::invalid_argument("ShaderProg");
	}

	glDeleteShader(shaderVertex);
	glDeleteShader(shaderFrag);
}

Shader::Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath) {
	std::filesystem::path vertex_FullPath = SHADER_DIR / vertexPath;
	std::filesystem::path fragment_FullPath = SHADER_DIR / fragmentPath;
	std::filesystem::path geometry_FullPath = SHADER_DIR / geometryPath;

	std::string vertexCode = ReadFile(vertex_FullPath.string().c_str());
	const char* vShaderCode = vertexCode.c_str();

	std::string fragmentCode = ReadFile(fragment_FullPath.string().c_str());
	const char* fShaderCode = fragmentCode.c_str();

	std::string geometryCode = ReadFile(geometry_FullPath.string().c_str());
	const char* gShaderCode = geometryCode.c_str();

	unsigned int shaderVertex = glCreateShader(GL_VERTEX_SHADER);  
	glShaderSource(shaderVertex, 1, &vShaderCode, NULL);
	glCompileShader(shaderVertex);

	unsigned int shaderFrag = glCreateShader(GL_FRAGMENT_SHADER);  
	glShaderSource(shaderFrag, 1, &fShaderCode, NULL);
	glCompileShader(shaderFrag);

	unsigned int shaderGeometrie = glCreateShader(GL_GEOMETRY_SHADER);  
	glShaderSource(shaderGeometrie, 1, &gShaderCode, NULL);
	glCompileShader(shaderGeometrie);


	if (AssertShader(shaderVertex) || AssertShader(shaderFrag) || AssertShader(shaderGeometrie))  
		throw std::invalid_argument("Chemin incorrect entrer dans le constructeur");

	shaderID = glCreateProgram();

	glAttachShader(shaderID, shaderVertex);  
	glAttachShader(shaderID, shaderGeometrie);
	glAttachShader(shaderID, shaderFrag);

	glLinkProgram(shaderID);


	//Assert du shaderProgram
	int success;
	char infoLog[512];
	glGetProgramiv(shaderID, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderID, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROG::CREATION_FAILED\n" << infoLog << std::endl;
		std::cout << "VertexPath :" << vertex_FullPath.string() << std::endl;
		std::cout << "FragPath :" << fragment_FullPath.string() << std::endl;
		std::cout << "GeometryPath :" << geometry_FullPath.string() << std::endl;
		throw std::invalid_argument("ShaderProg");
	}

	glDeleteShader(shaderVertex);
	glDeleteShader(shaderFrag);
	glDeleteShader(shaderGeometrie);
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

void Shader::CreateDefaultWhiteTexture() {
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	// CRÉATION DES DONNÉES DU PIXEL
	// R = 255, G = 255, B = 255, A = 255 (Blanc Opaque)
	unsigned char whitePixel[] = { 255, 255, 255, 255 };

	// ENVOI À OPENGL
	// 1x1 pixel, format RGBA
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, whitePixel);

	// CONFIGURATION (Important pour éviter des artefacts bizarres)
	// On veut que ce soit simple : pas de flou, répétition simple
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	defaultText = textureID;

	defaultText_BindlessHandle = glGetTextureHandleARB(textureID);
	glMakeTextureHandleResidentARB(defaultText_BindlessHandle);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void Shader::CreateNeutralNormalText() {
	unsigned char neutralNormal[] = { 128, 128, 255, 255 }; // RGBA

	GLuint neutralNormalTexture;
	glGenTextures(1, &neutralNormalTexture);
	glBindTexture(GL_TEXTURE_2D, neutralNormalTexture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, neutralNormal);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	neutralNormalText = neutralNormalTexture;

	neutralNormalText_BindlessHandle = glGetTextureHandleARB(neutralNormalText);
	glMakeTextureHandleResidentARB(neutralNormalText_BindlessHandle);

	glBindTexture(GL_TEXTURE_2D, 0);
}