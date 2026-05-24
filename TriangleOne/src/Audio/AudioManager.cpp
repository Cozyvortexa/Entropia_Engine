#include "Audio/AudioManager.h"
namespace Audio = Engine::Audio;

void Audio::AudioManager::SetEngineVolume(ma_engine& engine, const float value) {
    ma_engine_set_volume(&engine, value);
}


#pragma region Basic Method

void Audio::AudioManager::Start(Audio& audio) {
    ma_sound_start(audio.sound);
}

void Audio::AudioManager::Pause(Audio& audio) {
    if (ma_sound_is_playing(audio.sound)) {
        ma_sound_stop(audio.sound);
    }
    else {
        ma_sound_start(audio.sound);
    }
}
void Audio::AudioManager::Stop(Audio& audio) {
    ma_sound_stop(audio.sound);
}

void Audio::AudioManager::Restart(Audio& audio) {
    ma_sound_seek_to_pcm_frame(audio.sound, 0);
    ma_sound_start(audio.sound);
}

void Audio::AudioManager::SetVolume(Audio& audio, const float newValue) {
    if (newValue < 0.0f) {
        ma_sound_set_volume(audio.sound, 0.0f);
        return;
    }
    ma_sound_set_volume(audio.sound, newValue);
}

#pragma endregion


#pragma region Advance Methode

void Audio::AudioManager::SetLooping(Audio& audio, const bool newValue) {
    ma_sound_set_looping(audio.sound, newValue);
}

void Audio::AudioManager::SetPanning(Audio& audio, float newPanningValue) {
    if (newPanningValue > 1.0f) { newPanningValue = 1.0f; }
    else if (newPanningValue < -1.0f) { newPanningValue = -1.0; }

    ma_sound_set_pan(audio.sound, newPanningValue);
}

void Audio::AudioManager::SetPitch(Audio& audio, const float newPitchValue) {
    ma_sound_set_pitch(audio.sound, newPitchValue);
}

void Audio::AudioManager::SetDopplerEffect(Audio& audio, const float newValue) {
    ma_sound_set_doppler_factor(audio.sound, newValue);
}

//void Audio::AudioManager::AudioSetDirection(Audio& audio, glm::vec3 forward) {
//
//    AudioStruct* currentAudio = nullptr;
//    if (instance->AudioBrowser(name, currentAudio)) {
//        currentAudio->forwardX = forwardX;
//        currentAudio->forwardY = forwardY;
//        currentAudio->forwardZ = forwardZ;
//    }
//    audio.sound->forwardX
//}

void Audio::AudioManager::PlayerSeekTo(Audio& audio, const int timeInMs) {
    ma_uint32 sampleRate;
    ma_sound_get_data_format(audio.sound, nullptr, nullptr, &sampleRate, nullptr, 0);

    ma_sound_seek_to_pcm_frame(audio.sound, (unsigned long long)(timeInMs * sampleRate) / 1000);
}

//void Audio::AudioManager::test3D() {
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

void Audio::AudioManager::SetFadeIn(Audio& audio, float fadeInValue) {
    ma_sound_set_fade_in_milliseconds(audio.sound, 0, -1, (ma_uint64)fadeInValue);
}

void Audio::AudioManager::SetMinDistance(Audio& audio, const float minValue) {
    if (minValue < 0.0f) {
        ma_sound_set_min_distance(audio.sound, 0);
        return;
    }
    ma_sound_set_min_distance(audio.sound, minValue);
}

void Audio::AudioManager::SetMaxDistance(Audio& audio, const float maxValue) {
    ma_sound_set_max_distance(audio.sound, maxValue);
}

void Audio::AudioManager::SetVelocity(Audio& audio, float velocityX, float velocityY, float velocityZ) {
    ma_sound_set_velocity(audio.sound, velocityX, velocityY, velocityZ);
}


#pragma endregion


void Audio::AudioManager::DeleteSound(Audio* audio) {
    if (audio == nullptr || audio->sound == nullptr) return;

    ma_sound_stop(audio->sound);
    //ma_sound_uninit(audio->sound);

    delete audio->sound;
}