#pragma once
#include <string>

#include <Utilities/miniaudio.h>


namespace Engine::Audio {
	enum class SoundFlags : uint32_t {
		None = 0,
		Async = 1 << 0, // 1
		Preload = 1 << 1 // 2
	};
	enum class AudioLifetime
	{
		Both,
		Runtime,
		Editor
	};
	inline static SoundFlags operator|(SoundFlags a, SoundFlags b){
		return static_cast<SoundFlags>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
	}

	inline static bool HasFlag(SoundFlags value, SoundFlags flag){
		return (static_cast<uint32_t>(value) & static_cast<uint32_t>(flag)) != 0;
	}

	struct Audio{
		~Audio() = default;
		Audio() = default;
		Audio(std::string name, std::string path, ma_sound* sound, SoundFlags flags){
			this->name = name;
			this->path = path;
			this->sound = sound;
			this->flags = flags;
		}
		SoundFlags flags = SoundFlags::None;

		std::string name = "None";
		std::string path = "None";

		bool looping = false;
		bool spatialisation = true;

		ma_sound* sound = nullptr;
	};

	struct SoundKey {
		SoundKey(const char& path, SoundFlags flags){
			this->path = path;
			this->flags = flags;
		}
		std::string path;
		SoundFlags flags;

		bool operator==(const SoundKey& other) const {
			return path == other.path && flags == other.flags;
		}
	};
}


namespace std {
	template<>
	struct hash<Engine::Audio::SoundKey> {
		size_t operator()(const Engine::Audio::SoundKey& k) const {
			return std::hash<std::string>()(k.path) ^ (std::hash<uint32_t>()(static_cast<uint32_t>(k.flags)) << 1);
		}
	};
}
