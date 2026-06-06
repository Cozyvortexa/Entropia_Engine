#include "Systems/AudioSystem.h"

#define MINIAUDIO_IMPLEMENTATION
#include "Utilities/miniaudio.h"

namespace Systems = Engine::Systems;
namespace Resource = Engine::Resource;
namespace Audio = Engine::Audio;

void Systems::AudioSystem::UpdatePositionAudio(World* world, Resource::AudioResource* audioData) {
    View audio_Source_view = world->view<Component::Transform, Component::AudioSource>();
    View audio_Listener_view = world->view<Component::Transform, Component::AudioListener, Component::CameraComponent>();

    audio_Source_view.each([&](int entity, Component::Transform& transform, Component::AudioSource& audioSource) {
        if (audioSource.audio != nullptr && audioSource.audio->spatialisation) {
            ma_sound_set_position(audioSource.audio->sound, transform.position.x, transform.position.y, transform.position.z);
        }
    });

    audio_Listener_view.each([&](int entity, Component::Transform& transform, Component::AudioListener& audioListerner, Component::CameraComponent& cameraComponent) {
        ma_engine_listener_set_position(&audioData->audioEngine, audioListerner.index, cameraComponent.cameraFront.x, cameraComponent.cameraFront.y, cameraComponent.cameraFront.z);
    });
}

void Systems::AudioSystem::Init(World& world, const Resource::ResourceBuffer* resourceBuffer) {
    Resource::AudioResource* audioData = resourceBuffer->audioResource;

    ma_resource_manager_config rmConfig = ma_resource_manager_config_init();
    ma_result result = ma_resource_manager_init(&rmConfig, &audioData->resourceManager);

    ma_engine_config engineConfig = ma_engine_config_init();
    engineConfig.pResourceManager = &audioData->resourceManager;


    if (ma_engine_init(&engineConfig, &audioData->audioEngine) != ma_result::MA_SUCCESS) {
        std::cerr << "Failed to initialize the audio engine. " << std::endl;
        abort();  // Critical error
    }

    Engine::Audio::SetEngineVolume(audioData->audioEngine, 0.5f);
}

void Systems::AudioSystem::Update(World& world, const Resource::ResourceBuffer* resourceBuffer) {
    Resource::AudioResource* audioData = resourceBuffer->audioResource;
    UpdatePositionAudio(&world, audioData);
}

void Systems::AudioSystem::Shutdown(World& world) {
    Resource::AudioResource* audioData = world.get_ressource<Resource::AudioResource>();

    ma_engine_uninit(&audioData->audioEngine);

    // 2. On éteint le resource manager en deuxième
    ma_resource_manager_uninit(&audioData->resourceManager);

    std::cout << "Shut down AudioSystem" << std::endl;
}