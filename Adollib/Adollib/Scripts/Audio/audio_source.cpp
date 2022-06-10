
#include "audio_source.h"
#include "audio_manager.h"

#include "audio_data_struct.h"
#include "audio_emitter.h"

#include "../../DirectXTK-master/Inc/Audio.h"
#include "../../Imgui/imgui.h"

using namespace Adollib;

void Audio_source::awake() {
	emitter_data = std::make_shared<Audio_emitter>();
}

void Audio_source::update() {

	if (sound_instance == nullptr) {
		return;
	}

	// 3D座標の更新、など(Apply3DはReverbの更新もしてくれる)
	{
		if (is_useReverb) {
			if (is_use3D) {
				const auto& listener = Audio_manager::get_listener_data();
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

		if(is_mute) sound_instance->SetVolume(0);
		else sound_instance->SetVolume(volume);
	}

}

void Audio_source::finalize() {
	if (sound_instance == nullptr) {
		return;
	}
	sound_instance->Stop();
}

void Audio_source::load(const std::string& file_path, const bool is_use3D_, const bool is_useReverb_, const bool is_usePitchchenge_) {
	auto audio_data = Audio_manager::load(file_path);

	emitter_data = std::make_shared<Audio_emitter>();

	is_use3D = is_use3D_;
	is_useReverb = is_useReverb_;
	is_usePitchchenge = is_usePitchchenge_;

	Audio_manager::create_instance(sound_instance, audio_data->soundEffect, is_use3D | is_useReverb, is_useReverb, is_usePitchchenge);
}

void Audio_source::play(const bool is_loop) {
	if (sound_instance == nullptr) {
		assert(0 && "this AudioSorce didnt load the file,");
		return;
	}
	sound_instance->Play(is_loop);
}
void Audio_source::stop() {
	if (sound_instance == nullptr) {
		assert(0 && "this AudioSorce didnt load the file,");
		return;
	}
	sound_instance->Stop();
}
void Audio_source::pause() {
	if (sound_instance == nullptr) {
		assert(0 && "this AudioSorce didnt load the file,");
		return;
	}
	sound_instance->Pause();
}


void Audio_source::Update_hierarchy() {
	//float volume = 0.3f;
	//bool is_mute = false;
	//bool is_loop = false;
	//float pitch = 1;

	//Vector3 OrientFront_local; // orientation of front direction, used only for emitter angle calculations or with multi-channel emitters for matrix calculations or single-channel emitters with cones for matrix, LPF (both direct and reverb paths), and reverb calculations, must be normalized when used
	//Vector3 OrientTop_local;   // orientation of top direction, used only with multi-channel emitters for matrix calculations, must be orthonormal with OrientFront when used

	//Vector3 Position_local; // position in user-defined world units, does not affect Velocity
	//Vector3 Velocity_local; // velocity vector in user-defined world units/second, used only for doppler calculations, does not affect Position

	//float InnerRadius;      // inner radius, must be within [0.0f, FLT_MAX]
	//float InnerRadiusAngle; // inner radius angle, must be within [0.0f, X3DAUDIO_PI/4.0)

	//int ChannelCount;       // number of sound channels, must be > 0
	//float ChannelRadius;     // channel radius, used only with multi-channel emitters for matrix calculations, must be >= 0.0f when used
	//float* pChannelAzimuths; // channel azimuth array, used only with multi-channel emitters for matrix calculations, contains positions of each channel expressed in radians along the channel radius with respect to the front orientation vector in the plane orthogonal to the top orientation vector, or X3DAUDIO_2PI to specify an LFE channel, must have at least ChannelCount elements, all within [0.0f, X3DAUDIO_2PI] when used


	ImGui::DragFloat("volume", &volume, 0.01f, 0, 1);
	ImGui::Checkbox("is_mute", &is_mute);

	if(is_usePitchchenge)
	ImGui::DragFloat("pitch", &pitch, 0.01f, -1, 1);


	//ImGui::Checkbox("is_loop", &is_loop);
}