
#include "audio_resource_manager.h"

#include "../../DirectXTK-master/Audio/WAVFileReader.h"
#include "../../DirectXTK-master/Inc/Audio.h"

#include "audio_listener.h"

#include <memory>
#include <stdlib.h>

using namespace Adollib;
using namespace DirectX;

std::unordered_map<std::string, Audio_data> Audio_resource_manager::audio_datas = {};
std::unique_ptr<DirectX::AudioEngine> Audio_resource_manager::audioEngine = nullptr;
std::list<Audio_listener*> Audio_resource_manager::audio_listener_comps = {};
DirectX::AudioListener Audio_resource_manager::listener = {};

void Audio_resource_manager::start() {
    DirectX::AUDIO_ENGINE_FLAGS eflags =
        DirectX::AudioEngine_ReverbUseFilters |
        DirectX::AudioEngine_EnvironmentalReverb |
        DirectX::AudioEngine_UseMasteringLimiter;

#ifdef _DEBUG
    eflags |= DirectX::AudioEngine_Debug;
#endif;
    audioEngine = std::make_unique<AudioEngine>(eflags);

}
void Audio_resource_manager::update() {
    audioEngine->Update();

    // listenerdataの更新
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
void Audio_resource_manager::destroy() {
    audioEngine->Reset();
}


Audio_data* Audio_resource_manager::load(const std::string& file_path) {

    if (audio_datas.count(file_path) != 0)return &audio_datas[file_path];

    auto& data = audio_datas[file_path];
    //data.wavData = std::make_unique<uint8_t[]>();
    data.wavInfo = std::make_shared<DirectX::WAVData>();

    // stringからw_charに変換 XTK君がこっちを要求してくるため
    //wchar_t* wfile_name = nullptr;
    //std::mbstowcs_s(wfile_name, file_name.c_str(), sizeof(file_name.c_str()));
    const size_t newsizew = file_path.size() + 1;
    size_t convertedChars = 0;
    wchar_t* wfile_name = new wchar_t[newsizew];
    mbstowcs_s(&convertedChars, wfile_name, newsizew, file_path.c_str(), _TRUNCATE);


    if (wfile_name == nullptr) {
        assert(1 || "ERROR: SoundEffect failed (%08X) to load from .wav file");

        delete[]wfile_name;
        return &data;
    }

    // load
    HRESULT hr = LoadWAVAudioFromFileEx(wfile_name, data.wavData, *data.wavInfo);
    if (FAILED(hr))
    {
        assert(1 || "ERROR: SoundEffect failed (%08X) to load from .wav file");
    }

    delete[]wfile_name;

    // sound effectの生成 1つのwavから複数のinstnceの生成なんかやらないと思うからLoadと一緒に作っちゃう
    data.soundEffect = std::make_unique<DirectX::SoundEffect>(audioEngine.get(), data.wavData, data.wavInfo->wfx, data.wavInfo->startAudio, data.wavInfo->audioBytes, data.wavInfo->loopStart, data.wavInfo->loopLength);


    return &data;

}
void Audio_resource_manager::release(const std::string& file_path) {
    if (audio_datas.count(file_path) == 0)return;
    audio_datas.erase(file_path);
}

std::unique_ptr<DirectX::SoundEffectInstance>& Audio_resource_manager::create_instance(std::unique_ptr<DirectX::SoundEffect>& soundefect, const bool is_use3D, const bool is_useReverb , const bool is_usePitchchenge ) {

    SOUND_EFFECT_INSTANCE_FLAGS flag = DirectX::SOUND_EFFECT_INSTANCE_FLAGS::SoundEffectInstance_Default;
    if (is_use3D)            flag |= DirectX::SOUND_EFFECT_INSTANCE_FLAGS::SoundEffectInstance_Use3D;
    if (is_useReverb)        flag |= DirectX::SOUND_EFFECT_INSTANCE_FLAGS::SoundEffectInstance_ReverbUseFilters;
    if (!is_usePitchchenge)  flag |= DirectX::SOUND_EFFECT_INSTANCE_FLAGS::SoundEffectInstance_NoSetPitch;

    return soundefect->CreateInstance(flag);
}

std::list<Audio_listener*>::iterator Audio_resource_manager::add_listener_component(Audio_listener* comp) {
    audio_listener_comps.emplace_back(comp);
    auto itr = audio_listener_comps.end();
    --itr;
    return itr;
}
void Audio_resource_manager::remove_listener_component(std::list<Audio_listener*>::iterator itr) {
    audio_listener_comps.erase(itr);
}
void Audio_resource_manager::move_listener_end(std::list<Audio_listener*>::iterator itr) {
    auto end_itr = audio_listener_comps.end();
    --end_itr;

    auto ptr_save = *end_itr;

    *end_itr = *itr;
    *itr = ptr_save;
}
const DirectX::AudioListener& Audio_resource_manager::get_listener_data() {
    return listener;
}