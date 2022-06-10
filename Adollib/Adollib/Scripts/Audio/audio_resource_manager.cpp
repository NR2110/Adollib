
#include "audio_resource_manager.h"

#include "../../DirectXTK-master/Audio/WAVFileReader.h"
#include "../../DirectXTK-master/Inc/Audio.h"

#include "audio_listener.h"

#include <memory>
#include <stdlib.h>

using namespace Adollib;
using namespace DirectX;

std::unordered_map<std::string, Audio_data> Audio_resource_manager::audio_datas = {};


Audio_data* Audio_resource_manager::load(DirectX::AudioEngine* audioEngine, const std::string& file_path) {

    if (audio_datas.count(file_path) != 0)return &audio_datas[file_path];

    auto& data = audio_datas[file_path];
    //data.wavData = std::make_unique<uint8_t[]>();
    data.wavInfo = std::make_shared<DirectX::WAVData>();

    // string‚©‚çw_char‚É•ÏŠ· XTKŒN‚ª‚±‚Á‚¿‚ğ—v‹‚µ‚Ä‚­‚é‚½‚ß
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

    // sound effect‚Ì¶¬ 1‚Â‚Ìwav‚©‚ç•¡”‚Ìinstnce‚Ì¶¬‚È‚ñ‚©‚â‚ç‚È‚¢‚Æv‚¤‚©‚çLoad‚Æˆê‚Éì‚Á‚¿‚á‚¤
    data.soundEffect = std::make_unique<DirectX::SoundEffect>(audioEngine, data.wavData, data.wavInfo->wfx, data.wavInfo->startAudio, data.wavInfo->audioBytes, data.wavInfo->loopStart, data.wavInfo->loopLength);


    return &data;

}
void Audio_resource_manager::release(const std::string& file_path) {
    if (audio_datas.count(file_path) == 0)return;
    audio_datas.erase(file_path);
}