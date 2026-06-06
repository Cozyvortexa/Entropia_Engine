#pragma once
#include "Audio/AudioStruct.h"

namespace Engine::Audio {
		// Invidual Audio

	/*
		*@brief Start playing a sound
		*
		* @param name : name of the Audio
		*
		*/
	void Start(Audio& audio);
	/*
		*@brief Pause or restart a sound after pausing it
		*
		* @param name : name of the Audio
		*
		*/
	void Pause(Audio& audio);

	void Stop(Audio& audio);
	/*
		*@brief Restart a sound from the beginning
		*
		* @param name : name of the Audio
		*
		*/
	void Restart(Audio& audio);
	/*
		*@brief Adjust the volume of a sound by multiplicative value, if the value is greater than 1 the sound will be amplified and 0 silences him
		*
		* @param name : name of the Audio
		*
		* @param newValue : the new multiplier
		*
		*/
	void SetVolume(Audio& audio, float newValue);
	/*
		*@brief With this parameter is activated, the sound will loop indefinitely
		*
		* @param name : name of the Audio
		*
		* @param newValue : pass to true or false
		*
		*/
	void SetLooping(Audio& audio, bool newValue);
	/*
		*@brief Setting the pan to 0 will result in an unpanned sound. Setting it to -1 will shift everything to the left, whereas +1 will shift it to the right
		*
		* @param name : name of the Audio
		*
		* @param newPanningValue : define the new Panning Value cap between -1 and 1 inclusive
		*
		*/
	void SetPanning(Audio& audio, float newPanningValue);
	/*
		*@brief A larger value will result in a higher pitch.The pitch must be greater than 0.
		*
		* @param name : name of the Audio
		*
		* @param newPitchValue : define the new pitch Value
		*
		*/
	void SetPitch(Audio& audio, float newPitchValue);

	void SetDopplerEffect(Audio& audio, float newValue); // Not tested, requires velocity
	/*
		*@brief Move playback position to timeInMs milliseconds
		*
		* @param name : name of the Audio
		*
		* @param timeInMs : value where the reader will be moved
		*
		*/
	void PlayerSeekTo(Audio& audio, int timeInMs);

	void SetFadeIn(Audio& audio, float fadeInValue);

	void SetMinDistance(Audio& audio, float minValue);

	void SetMaxDistance(Audio& audio, float maxValue);

	void SetVelocity(Audio& audio, float velocityX, float velocityY, float velocityZ);

	void SetEngineVolume(ma_engine& engine, const float value);
	void DeleteSound(Audio* audio);
}
