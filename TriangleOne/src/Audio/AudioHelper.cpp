#include "Audio/AudioHelper.h"
#include <iostream>
namespace Audio = Engine::Audio;

void Audio::SetEngineVolume(ma_engine& engine, const float value) {
    ma_engine_set_volume(&engine, value);
}

// Return True when the sound is not a null ptr otherwise, false
bool const inline ChekSoundValidity(ma_sound* sound) {
    if (sound == nullptr) {
        std::cout << "Null sound detected" << std::endl;
        return false;
    }
    return true;
}

#pragma region Basic Method

void Audio::Start(Audio& audio) {
    if (!ChekSoundValidity(audio.sound)) return;

    ma_sound_start(audio.sound);
}

void Audio::Pause(Audio& audio) {
    if (!ChekSoundValidity(audio.sound)) return;

    if (ma_sound_is_playing(audio.sound)) {
        ma_sound_stop(audio.sound);
    }
    else {
        ma_sound_start(audio.sound);
    }
}
void Audio::Stop(Audio& audio) {
    if (!ChekSoundValidity(audio.sound)) return;

    ma_sound_stop(audio.sound);
}

void Audio::Restart(Audio& audio) {
    if (!ChekSoundValidity(audio.sound)) return;

    ma_sound_seek_to_pcm_frame(audio.sound, 0);
    ma_sound_start(audio.sound);
}

void Audio::SetVolume(Audio& audio, const float newValue) {
    if (!ChekSoundValidity(audio.sound)) return;

    if (newValue < 0.0f) {
        ma_sound_set_volume(audio.sound, 0.0f);
        return;
    }
    ma_sound_set_volume(audio.sound, newValue);
}

#pragma endregion


#pragma region Advance Methode

void Audio::SetLooping(Audio& audio, const bool newValue) {
    if (!ChekSoundValidity(audio.sound)) return;

    ma_sound_set_looping(audio.sound, newValue);
}

void Audio::SetPanning(Audio& audio, float newPanningValue) {
    if (!ChekSoundValidity(audio.sound)) return;

    if (newPanningValue > 1.0f) { newPanningValue = 1.0f; }
    else if (newPanningValue < -1.0f) { newPanningValue = -1.0; }

    ma_sound_set_pan(audio.sound, newPanningValue);
}

void Audio::SetPitch(Audio& audio, const float newPitchValue) {
    if (!ChekSoundValidity(audio.sound)) return;

    ma_sound_set_pitch(audio.sound, newPitchValue);
}

void Audio::SetDopplerEffect(Audio& audio, const float newValue) {
    if (!ChekSoundValidity(audio.sound)) return;

    ma_sound_set_doppler_factor(audio.sound, newValue);
}

//void Audio::AudioSetDirection(Audio& audio, glm::vec3 forward) {
//
//    AudioStruct* currentAudio = nullptr;
//    if (instance->AudioBrowser(name, currentAudio)) {
//        currentAudio->forwardX = forwardX;
//        currentAudio->forwardY = forwardY;
//        currentAudio->forwardZ = forwardZ;
//    }
//    audio.sound->forwardX
//}

void Audio::PlayerSeekTo(Audio& audio, const int timeInMs) {
    if (!ChekSoundValidity(audio.sound)) return;

    ma_uint32 sampleRate;
    ma_sound_get_data_format(audio.sound, nullptr, nullptr, &sampleRate, nullptr, 0);

    ma_sound_seek_to_pcm_frame(audio.sound, (unsigned long long)(timeInMs * sampleRate) / 1000);
}

//void Audio::test3D() {
//
//    ma_engine_listener_set_position(&engine, 0, 0, 0, 0);
//
//    AudioStruct* currentAudio = nullptr;
//    if (AudioBrowser("mario64", currentAudio)) {
//        ma_sound_set_positioning(audio.sound, ma_positioning::ma_positioning_absolute);
//        ma_sound_set_position(audio.sound, 1, 0, 0);
//        //ma_sound_set_direction(audio.sound, 1, 0, 0);
//    }
//}

void Audio::SetFadeIn(Audio& audio, float fadeInValue) {
    if (!ChekSoundValidity(audio.sound)) return;

    ma_sound_set_fade_in_milliseconds(audio.sound, 0, -1, (ma_uint64)fadeInValue);
}

void Audio::SetMinDistance(Audio& audio, const float minValue) {
    if (!ChekSoundValidity(audio.sound)) return;

    if (minValue < 0.0f) {
        ma_sound_set_min_distance(audio.sound, 0);
        return;
    }
    ma_sound_set_min_distance(audio.sound, minValue);
}

void Audio::SetMaxDistance(Audio& audio, const float maxValue) {
    if (!ChekSoundValidity(audio.sound)) return;

    ma_sound_set_max_distance(audio.sound, maxValue);
}

void Audio::SetVelocity(Audio& audio, float velocityX, float velocityY, float velocityZ) {
    if (!ChekSoundValidity(audio.sound)) return;

    ma_sound_set_velocity(audio.sound, velocityX, velocityY, velocityZ);
}


#pragma endregion


void Audio::DeleteSound(Audio* audio) {
    if (audio == nullptr || audio->sound == nullptr) return;

    ma_sound_stop(audio->sound);
    ma_sound_uninit(audio->sound);  // Causes an error when the app is closed while playing an audio file

    delete audio->sound;
}