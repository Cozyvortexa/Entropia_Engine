#pragma once

#include "Audio/AudioHelper.h"

#include "Utilities/Observer.h"
#include "ECS/Components/ComponentBase.h"

namespace Engine::Component {
	struct AudioSource : public Component {
	public:
		~AudioSource() {
			Engine::Audio::DeleteSound(audio);
		}
		AudioSource() {
			audio = new Engine::Audio::Audio();
			SetupConnections();
			volume.Set(1.0f);
			range.Set(10.0f);
		};
		AudioSource(std::string name, std::string path, Engine::Audio::Audio* audio) {
			this->name = name;
			this->path = path;
			this->audio = audio;

			SetupConnections();
			volume.Set(1.0f);
			range.Set(10.0f);
		}

		AudioSource(AudioSource&& other) noexcept : name(std::move(other.name)), path(std::move(other.path)), audio(std::exchange(other.audio, nullptr)) {
			volume.Set(other.volume.Get());
			range.Set(other.range.Get());

			other.audio = nullptr;

			SetupConnections();
		}
		AudioSource& operator=(AudioSource&& other) noexcept {
			if (this != &other) {

				if (this->audio) {
					Engine::Audio::DeleteSound(this->audio);
				}
				//Clear current connection
				volumeConnection = ScopedConnection();
				rangeConnection = ScopedConnection();

				//Transfers the master data
				name = std::move(other.name);
				path = std::move(other.path);

				audio = (std::exchange(other.audio, nullptr));

				//Synchronises the values of the observers
				volume.Set(other.volume.Get());
				range.Set(other.range.Get());

				SetupConnections();
			}
			return *this;
		}

		// Copy is prohibidden
		AudioSource(const AudioSource&) = delete;
		AudioSource& operator=(const AudioSource&) = delete;


		std::string path = "";
		std::string name = "None";
		Engine::Audio::Audio* audio;

		Observer<float> volume;
		Observer<float> range;

		template<typename F>
		void Reflect(F&& f)
		{
			f("Volume", volume);
			f("range", range);
			f("Audio", audio);
		}

	private:
		void SetupConnections() {
			volumeConnection = volume.Subscribe([this](const float& v) {
				if (this->audio) Audio::SetVolume(*(this->audio), v);
				});
			rangeConnection = range.Subscribe([this](const float& v) {
				if (this->audio) Audio::SetMaxDistance(*(this->audio), v);
				});
		}

		ScopedConnection volumeConnection;
		ScopedConnection rangeConnection;
	};

	struct AudioListener : public Component {
		ma_uint32 index = 0;
	};
}