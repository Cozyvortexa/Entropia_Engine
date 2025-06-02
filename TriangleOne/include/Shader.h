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
	Shader(const char* vertexPath, const char* fragmentPath);


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
			std::cerr << "Uniform: " << name << std::endl;
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
	}
	void setMatrix(const std::string& name, glm::mat3 matrix)
	{
		unsigned int projLoc = glGetUniformLocation(shaderID, name.c_str());
		glUniformMatrix3fv(projLoc, 1, GL_FALSE, glm::value_ptr(matrix));
	}


	unsigned int shaderID = NULL;

private:
	std::string ReadFile(const char* shaderPath);
	int AssertShader(unsigned int& shader);
};

