
#include "demo_music.h"
#include <unordered_map>

#include "../Adollib/Scripts/Object/gameobject_manager.h"

#include "../Adollib/Scripts/Renderer/renderer_base.h"

#include "../Adollib/Scripts/Imgui/work_meter.h"

#include "../Adollib/Scripts/Main/systems.h"

#include "../Adollib/Imgui/imgui.h"
#include "../../Scripts/Main/mono_audio.h"

using namespace Adollib;
using namespace DirectX;

namespace DirectX
{
	class AudioEngine;
	class SoundEffect;
	class WaveBank;
	class SoundEffectInstance;

}


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

void Demo_music::awake() {

	// Create DirectXTK for Audio objects
	//DirectX::AUDIO_ENGINE_FLAGS eflags = DirectX::AudioEngine_Default;
	DirectX::AUDIO_ENGINE_FLAGS eflags =
		DirectX::AudioEngine_ReverbUseFilters |
		DirectX::AudioEngine_EnvironmentalReverb |
		DirectX::AudioEngine_UseMasteringLimiter;

	audioEngine = std::make_unique<AudioEngine>(eflags);
	audioEngine->SetReverb(AUDIO_ENGINE_REVERB::Reverb_LargeRoom);
	//audioEngine->SetReverb(AUDIO_ENGINE_REVERB::Reverb_Default);

	soundeffect = new SoundEffect(audioEngine.get(), L"../Data/sounds/Demo_BGM/Theme_of_Rabi-Ribi.wav");

	audioInstance = soundeffect->CreateInstance(DirectX::SoundEffectInstance_Use3D | DirectX::SoundEffectInstance_ReverbUseFilters);
	//audioInstance = soundeffect->CreateInstance(DirectX::SoundEffectInstance_Default);
	audioInstance->Play(true);
	audioInstance->SetVolume(0.12f);
}

void Demo_music::update() {
	audioEngine->Update();

	if (audioEngine->IsCriticalError()) {
		assert(!audioEngine->IsCriticalError() && "audioengine critial error");
	}

	ImGuiWindowFlags flag = 0;
	//flag |= ImGuiWindowFlags_AlwaysAutoResize;
	flag |= ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysAutoResize;
	flag |= ImGuiWindowFlags_::ImGuiWindowFlags_NoDocking;
	if (ImGui::Begin("Music_demo", 0, flag)) {

		// reserve_type
		{
			static int R = DirectX::AUDIO_ENGINE_REVERB::Reverb_LargeRoom;
			int ago = R;
			ImGui::InputInt("Reserve", &R, 1);
			if (R < 0)R = 0;
			if (DirectX::AUDIO_ENGINE_REVERB::Reverb_MAX <= R)R = DirectX::AUDIO_ENGINE_REVERB::Reverb_MAX - 1;
			if (ago != R) {
				audioEngine->SetReverb(static_cast<DirectX::AUDIO_ENGINE_REVERB>(R));
				audioInstance->SetVolume(0.12f);
				if (audioInstance->GetState() != SoundState::PLAYING) {
					assert(0 && "error");
				}
			}
		}

		// listener_pos
		{
			static Vector3 pos;
			ImGui::DragFloat3("my_pos", &pos.x, 0.1f, -10, 10);
			listener.Position = pos;
		}

		// sound_pos
		{
			static Vector3 pos;
			ImGui::DragFloat3("sound_pos", &pos.x, 0.1f, -10, 10);
			emitter.Position = pos;
		}


	}
	ImGui::End();

	//audioInstance->Apply3D(listener, emitter);

}
