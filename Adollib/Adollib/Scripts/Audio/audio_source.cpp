
#include "audio_source.h"
#include "audio_resource_manager.h"

#include "audio_data_struct.h"
#include "audio_emitter.h"

#include "../../DirectXTK-master/Inc/Audio.h"

using namespace Adollib;

void Audio_source::awake() {
	emitter_data = std::make_shared<Audio_emitter>();
}

void Audio_source::update() {

	// 3D座標の更新、など(Apply3DはReverbの更新もしてくれる)
	if(!(sound_instance == nullptr))
	{
		if (is_useReverb) {
			if (is_use3D) {
				const auto listener = Audio_resource_manager::get_listener_data();
				DirectX::AudioEmitter emitter;
				emitter_data->convert_to_DirectXAudioEmitter(&emitter, transform->position, transform->orientation);
				sound_instance->Apply3D(listener, emitter);
			}
			else {
				DirectX::AudioListener default_listener;
				DirectX::AudioEmitter default_emitter;
				sound_instance->Apply3D(default_listener, default_emitter);
			}
		}

		if(is_usePitchchenge)
			sound_instance->SetPitch(pitch);

		sound_instance->SetPan(pan);

		if(is_mute) sound_instance->SetVolume(0);
		else sound_instance->SetVolume(volume);
	}

}

void Audio_source::load(const std::string& file_path, const bool is_use3D_, const bool is_useReverb_, const bool is_usePitchchenge_) {
	auto audio_data = Audio_resource_manager::load(file_path);

	emitter_data = std::make_shared<Audio_emitter>();

	is_use3D = is_use3D_;
	is_useReverb = is_useReverb_;
	is_usePitchchenge = is_usePitchchenge_;

	Audio_resource_manager::create_instance(audio_data->soundEffect, is_use3D, is_useReverb, is_usePitchchenge).swap(sound_instance);
}

void Audio_source::play(const bool is_loop) {
	sound_instance->Play(is_loop);
}