#pragma once
#include "string.h"
#include "Render/Shader.h"

class Texture;
struct Material {
	Material() = default;
	Material(Shader* shader) {
		this->shader = shader;
	}
	Material(std::string name, Shader* shader) {
		this->shader = shader;
		this->name = name;
	}
	Material(std::string name, const char* vertexPath, const char* fragmentPath) {
		shader = new Shader(vertexPath, fragmentPath);
		this->name = name;
	};

	std::string name = "DefaultName";
	Shader* shader;

	std::shared_ptr<Texture> diffuse_Text_Ptr = std::make_shared<Texture>();
	std::shared_ptr<Texture> normal_Text_Ptr = std::make_shared<Texture>();
	std::shared_ptr<Texture> ambientOcclusion_Text_Ptr = std::make_shared<Texture>();
	std::shared_ptr<Texture> roughness_handle_Text_Ptr = std::make_shared<Texture>();
	std::shared_ptr<Texture> metalness_handle_Text_Ptr = std::make_shared<Texture>();

	bool hasARM_Text = false;

	float ao_Factor = 1.0;
	float roughness_Factor = 0.5;
	float metallic_Factor = 0.0;

};

struct MaterialKey {
	MaterialKey() = default;
	MaterialKey(Texture* diffuse, Texture* normal, Texture* ambientOcclusion, Texture* metalness_handle, Texture* roughness_handle) {
		this->diffuse = (size_t)diffuse;
		this->normal = (size_t)normal;
		this->ambientOcclusion = (size_t)ambientOcclusion;
		this->metalness_handle = (size_t)metalness_handle;
		this->roughness_handle = (size_t)roughness_handle;
	}
	size_t diffuse = 0;
	size_t normal = 0;
	size_t ambientOcclusion = 0;
	size_t metalness_handle = 0;
	size_t roughness_handle = 0;

	bool operator==(const MaterialKey& other) const {  // Allows comparisons with other keys
		return diffuse == other.diffuse &&
			normal == other.normal &&
			ambientOcclusion == other.ambientOcclusion &&
			metalness_handle == other.metalness_handle &&
			roughness_handle == other.roughness_handle;
	}

};

namespace std {
	template<>
	struct hash<MaterialKey> {  // Add the way to hash MaterialKey
		size_t operator()(const MaterialKey& k) const { // Hashing
			size_t h1 = std::hash<size_t>()(k.diffuse);
			size_t h2 = std::hash<size_t>()(k.normal);
			size_t h3 = std::hash<size_t>()(k.ambientOcclusion);
			size_t h4 = std::hash<size_t>()(k.metalness_handle);
			size_t h5 = std::hash<size_t>()(k.roughness_handle);

			size_t hash = 0;
			hash ^= h1 + 0x9e3779b9 + (hash << 6) + (hash >> 2);  // 0x9e3779b9  is from the golden ratio
			hash ^= h2 + 0x9e3779b9 + (hash << 6) + (hash >> 2);  // ^ = XOR
			hash ^= h3 + 0x9e3779b9 + (hash << 6) + (hash >> 2);
			hash ^= h4 + 0x9e3779b9 + (hash << 6) + (hash >> 2);
			hash ^= h5 + 0x9e3779b9 + (hash << 6) + (hash >> 2);

			return hash;
		}
	};
}