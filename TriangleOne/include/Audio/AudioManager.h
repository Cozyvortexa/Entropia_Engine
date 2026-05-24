#pragma once
#include "ECS/Components/AudioStruct.h"

namespace Engine::Audio {
	class AudioManager {
	public:
		AudioManager() = default;
		~AudioManager() = default;
		// Invidual Audio

		/*
		*@brief Start playing a sound
		*
		* @param name : name of the Audio
		*
		*/
		static void Start(Audio& audio);
		/*
		*@brief Pause or restart a sound after pausing it
		*
		* @param name : name of the Audio
		*
		*/
		static void Pause(Audio& audio);

		static void Stop(Audio& audio);
		/*
		*@brief Restart a sound from the beginning
		*
		* @param name : name of the Audio
		*
		*/
		static void Restart(Audio& audio);
		/*
		*@brief Adjust the volume of a sound by multiplicative value, if the value is greater than 1 the sound will be amplified and 0 silences him
		*
		* @param name : name of the Audio
		*
		* @param newValue : the new multiplier
		*
		*/
		static void SetVolume(Audio& audio, float newValue);
		/*
		*@brief With this parameter is activated, the sound will loop indefinitely
		*
		* @param name : name of the Audio
		*
		* @param newValue : pass to true or false
		*
		*/
		static void SetLooping(Audio& audio, bool newValue);
		/*
		*@brief Setting the pan to 0 will result in an unpanned sound. Setting it to -1 will shift everything to the left, whereas +1 will shift it to the right
		*
		* @param name : name of the Audio
		*
		* @param newPanningValue : define the new Panning Value cap between -1 and 1 inclusive
		*
		*/
		static void SetPanning(Audio& audio, float newPanningValue);
		/*
		*@brief A larger value will result in a higher pitch.The pitch must be greater than 0.
		*
		* @param name : name of the Audio
		*
		* @param newPitchValue : define the new pitch Value
		*
		*/
		static void SetPitch(Audio& audio, float newPitchValue);

		static void SetDopplerEffect(Audio& audio, float newValue); // Non tester, neccesite de la velociter sur le son 
		/*
		*@brief Move playback position to timeInMs milliseconds
		*
		* @param name : name of the Audio
		*
		* @param timeInMs : value where the reader will be moved
		*
		*/
		static void PlayerSeekTo(Audio& audio, int timeInMs);

		static void SetFadeIn(Audio& audio, float fadeInValue);

		static void SetMinDistance(Audio& audio, float minValue);

		static void SetMaxDistance(Audio& audio, float maxValue);

		static void SetVelocity(Audio& audio, float velocityX, float velocityY, float velocityZ);

		static void SetEngineVolume(ma_engine& engine, const float value);

		static void DeleteSound(Audio* audio);
	};
}
