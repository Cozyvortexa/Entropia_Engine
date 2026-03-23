#pragma once
#include <glad/glad/glad.h> 
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader
{
public:
	Shader() {};
	Shader(const char* vertexPath, const char* fragmentPath);
	Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath);

	void Use();

	void setBool(const std::string& name, bool value) const
	{
		glUniform1i(glGetUniformLocation(shaderID, name.c_str()), (int)value);
	}
	void setInt(const std::string& name, int value) const
	{
		glUniform1i(glGetUniformLocation(shaderID, name.c_str()), value);

		GLint loc = glGetUniformLocation(shaderID, name.c_str());
		if (loc == -1) {
			std::cerr << "Uniform: " << name << " not found in shader!" << std::endl;
		}
	}
	void setFloat(const std::string& name, float value) const
	{
		glUniform1f(glGetUniformLocation(shaderID, name.c_str()), value);
	}

	
	void setVec3(const std::string& name, glm::vec3 value) 
	{
		glUniform3fv(glGetUniformLocation(shaderID, name.c_str()), 1, glm::value_ptr(value));

		GLint loc = glGetUniformLocation(shaderID, name.c_str());
		if (loc == -1) {
			std::cerr << "Uniform " << name << " not found in shader!" << std::endl;
		}
	}

	void setMatrix(const std::string& name, glm::mat4 matrix)
	{
		unsigned int projLoc = glGetUniformLocation(shaderID, name.c_str());
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(matrix));
		if (projLoc == -1) {
			std::cerr << "Uniform " << name << " not found in shader!" << std::endl;
		}
	}
	void setMatrix(const std::string& name, glm::mat3 matrix)
	{
		unsigned int projLoc = glGetUniformLocation(shaderID, name.c_str());
		glUniformMatrix3fv(projLoc, 1, GL_FALSE, glm::value_ptr(matrix));
		if (projLoc == -1) {
			std::cerr << "Uniform " << name << " not found in shader!" << std::endl;
		}
	}

	void DebugValueInShader() {
		GLint count;
		GLint size; // taille de la variable
		GLenum type; // type de la variable (GL_FLOAT, GL_SAMPLER_2D, etc.)
		const GLsizei bufSize = 32; // longueur max du nom
		GLchar name[bufSize]; // nom de la variable
		GLsizei length; // longueur du nom

		glGetProgramiv(shaderID, GL_ACTIVE_UNIFORMS, &count);
		std::cout << "--- Uniforms Actifs pour Shader ID " << shaderID << " ---" << std::endl;

		for (int i = 0; i < count; i++)
		{
			glGetActiveUniform(shaderID, (GLuint)i, bufSize, &length, &size, &type, name);

			// On récupère sa location officielle
			GLint location = glGetUniformLocation(shaderID, name);

			std::cout << "Uniform #" << i << " | Location: " << location << " | Nom: " << name << std::endl;
		}
		std::cout << "-----------------------------------------------" << std::endl;
	}

	static void CreateDefaultWhiteTexture();
	static void CreateNeutralNormalText();

	unsigned int shaderID = NULL;
	static unsigned int GetDefaultText() { assert(defaultText != 0); return defaultText; }
	static GLuint GetNeutralNormalText() { assert(neutralNormalText != 0); return neutralNormalText; }
private:
	std::string ReadFile(const char* shaderPath);
	int AssertShader(unsigned int& shader);

	static GLuint neutralNormalText;
	static unsigned int defaultText;
};

