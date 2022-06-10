
#include "audio_manager.h"

#include "../../DirectXTK-master/Audio/WAVFileReader.h"
#include "../../DirectXTK-master/Inc/Audio.h"

#include "audio_resource_manager.h"
#include "audio_listener.h"

#include <memory>
#include <stdlib.h>

using namespace Adollib;
using namespace DirectX;

std::unique_ptr<DirectX::AudioEngine> Audio_manager::audioEngine = nullptr;
std::list<Audio_listener*> Audio_manager::audio_listener_comps = {};
DirectX::AudioListener Audio_manager::listener = {};

void Audio_manager::initialize() {
    DirectX::AUDIO_ENGINE_FLAGS eflags =
        DirectX::AudioEngine_ReverbUseFilters |
        DirectX::AudioEngine_EnvironmentalReverb |
        DirectX::AudioEngine_UseMasteringLimiter;

#ifdef _DEBUG
    eflags |= DirectX::AudioEngine_Debug;
#endif;
    audioEngine = std::make_unique<AudioEngine>(eflags);

}
void Audio_manager::update() {
    audioEngine->Update();

    // listenerdata‚ÌXV
    {
        if (audio_listener_comps.size() == 0) {
            listener.Position = Vector3(0);
            listener.Velocity = Vector3(0);
            listener.OrientFront = Vector3(0);
            listener.OrientTop = Vector3(0);
            listener.pCone = nullptr;
        }
        else {
            auto itr = audio_listener_comps.end();
            --itr;
            (*itr)->convert_to_DirectXAudioListener(&listener);
        }
    }

}
void Audio_manager::destroy() {
    //audioEngine->Reset();
}


Audio_data* Audio_manager::load(const std::string& file_path) {
    return Audio_resource_manager::load(audioEngine.get(), file_path);
}
void Audio_manager::release(const std::string& file_path) {
    return Audio_resource_manager::release(file_path);
}

void Audio_manager::create_instance(std::unique_ptr<DirectX::SoundEffectInstance>& out, std::unique_ptr<DirectX::SoundEffect>& soundefect, const bool is_use3D, const bool is_useReverb, const bool is_usePitchchenge) {

    SOUND_EFFECT_INSTANCE_FLAGS flag = DirectX::SOUND_EFFECT_INSTANCE_FLAGS::SoundEffectInstance_Default;
    if (is_use3D)            flag |= DirectX::SOUND_EFFECT_INSTANCE_FLAGS::SoundEffectInstance_Use3D;
    if (is_useReverb)        flag |= DirectX::SOUND_EFFECT_INSTANCE_FLAGS::SoundEffectInstance_ReverbUseFilters;
    //if (!is_usePitchchenge)  flag |= DirectX::SOUND_EFFECT_INSTANCE_FLAGS::SoundEffectInstance_NoSetPitch;

    out = soundefect->CreateInstance(flag);
}

std::list<Audio_listener*>::iterator Audio_manager::add_listener_component(Audio_listener* comp) {
    audio_listener_comps.emplace_back(comp);
    auto itr = audio_listener_comps.end();
    --itr;
    return itr;
}
void Audio_manager::remove_listener_component(std::list<Audio_listener*>::iterator itr) {
    audio_listener_comps.erase(itr);
}
void Audio_manager::move_listener_end(std::list<Audio_listener*>::iterator itr) {
    auto end_itr = audio_listener_comps.end();
    --end_itr;

    auto ptr_save = *end_itr;

    *end_itr = *itr;
    *itr = ptr_save;
}
const DirectX::AudioListener& Audio_manager::get_listener_data() {
    return listener;
}

void Audio_manager::set_reverb(const XAUDIO2FX_REVERB_PARAMETERS* native) {
    audioEngine->SetReverb(native);
}