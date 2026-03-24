#pragma once
#include "string.h"
#include "Render/Shader.h"

struct Material {
	Material(Shader shader) {
		this->shader = shader;
	}
	Material(std::string name, Shader shader) {
		this->shader = shader;
		this->name = name;
	}
	Material(std::string name, const char* vertexPath, const char* fragmentPath) {
		shader = Shader(vertexPath, fragmentPath);
		this->name = name;
	};

	std::string name = "DefaultName";
	Shader shader;

	unsigned int diffuse_Text_Handle = -1;
	unsigned int specular_Text_Handle = -1;
	unsigned int normal_Text_Handle = -1;
};

struct MaterialKey {
	MaterialKey() = default;
	MaterialKey(unsigned int diffuse, unsigned int specular, unsigned int normal) {
		this->diffuse = diffuse;
		this->specular = specular;
		this->normal = normal;
	}
	size_t diffuse = 0;
	size_t specular = 0;
	size_t normal = 0;

	bool operator==(const MaterialKey& other) const {  // Allows comparisons with other keys
		return diffuse == other.diffuse &&
			specular == other.specular &&
			normal == other.normal;
	}

};

namespace std {
	template<>
	struct hash<MaterialKey> {  // Add the way to hash MaterialKey
		size_t operator()(const MaterialKey& k) const { // Hashing
			size_t h1 = std::hash<size_t>()(k.diffuse);
			size_t h2 = std::hash<size_t>()(k.normal);
			size_t h3 = std::hash<size_t>()(k.specular);

			size_t hash = 0;
			hash ^= h1 + 0x9e3779b9 + (hash << 6) + (hash >> 2);  // 0x9e3779b9  is from the golden ratio
			hash ^= h2 + 0x9e3779b9 + (hash << 6) + (hash >> 2);  // ^ = XOR
			hash ^= h3 + 0x9e3779b9 + (hash << 6) + (hash >> 2);

			return hash;
		}
	};
}