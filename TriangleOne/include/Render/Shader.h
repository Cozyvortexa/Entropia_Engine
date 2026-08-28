#pragma once
#include <glad/glad/glad.h> 
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <unordered_map>

#include "PathFolder.h"

#include <cstdint>

class Shader{
public:
	Shader() {};
	//The path starts with the Shader folder
	Shader(const char* vertexPath, const char* fragmentPath);
	//The path starts with the Shader folder
	Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath);
	//~Shader() = default;
	~Shader() {
		if (shaderID != 0) {
			glUseProgram(0);
			glDeleteProgram(shaderID);
			shaderID = NULL;
		}
	}

	Shader(const Shader&) = delete;
	Shader& operator=(const Shader&) = delete;

	void Use();

	inline void setBool(const std::string& name, bool value) const
	{
		glUniform1i(GetUniformLocation(name), value);
	}
	inline void setInt(const std::string& name, int value) const
	{
		glUniform1i(GetUniformLocation(name), value);
	}
	inline void setFloat(const std::string& name, float value) const
	{
		glUniform1f(GetUniformLocation(name), value);
	}

	inline void setVec(const std::string& name, glm::vec2 value) //Vec2
	{
		glUniform2fv(GetUniformLocation(name), 1, glm::value_ptr(value));
	}

	inline void setVec(const std::string& name, glm::vec3 value) //Vec3
	{
		glUniform3fv(GetUniformLocation(name), 1, glm::value_ptr(value));
	}

	inline void setVec(const std::string& name, std::vector<glm::vec3> values)
	{
		glUniform3fv(GetUniformLocation(name), 1, glm::value_ptr(values[0]));
	}

	inline void setVec(const std::string& name, glm::vec4 value) //Vec4
	{
		glUniform4fv(GetUniformLocation(name), 1, glm::value_ptr(value));
	}

	inline void setMatrix(const std::string& name, glm::mat4 matrix)
	{
		glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(matrix));
	}
	inline void setMatrix(const std::string& name, glm::mat3 matrix)
	{
		glUniformMatrix3fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void DebugValueInShader() {
		GLint count;
		GLint size; // variable size
		GLenum type; // variable type (GL_FLOAT, GL_SAMPLER_2D,...)
		const GLsizei bufSize = 32; // maximum name length
		GLchar name[bufSize]; // variable name
		GLsizei length; // name length

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
	static uint32_t GetNeutralNormalText() { assert(neutralNormalText != 0); return neutralNormalText; }

	//BindlessHandle
	static uint64_t GetDefaultText_Handle() { assert(defaultText_BindlessHandle != 0); return defaultText_BindlessHandle; }
	static uint64_t GetNeutralNormalText_Handle() { assert(neutralNormalText_BindlessHandle != 0); return neutralNormalText_BindlessHandle; }
private:
	std::string ReadFile(const char* shaderPath);
	int AssertShader(unsigned int& shader);

	inline static uint32_t neutralNormalText = 0;
	inline static uint64_t neutralNormalText_BindlessHandle = 0;

	inline static unsigned int defaultText = 0;
	inline static uint64_t defaultText_BindlessHandle = 0;

	mutable std::unordered_map<std::string, GLint> uniformLocationCache;

	GLint GetUniformLocation(const std::string& name) const {
		auto it = uniformLocationCache.find(name);
		if (it != uniformLocationCache.end())
			return it->second;

		GLint location = glGetUniformLocation(shaderID, name.c_str());
		uniformLocationCache[name] = location;

		#ifndef NDEBUG
		if (location == -1) {
			std::cerr << "Uniform " << name << " not found in shader!" << std::endl;
		}
		#endif

		return location;
	}
};

