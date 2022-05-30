
#include "demo_music.h"
#include <unordered_map>

#include "../Adollib/Scripts/Object/gameobject_manager.h"

#include "../Adollib/Scripts/Renderer/renderer_base.h"

#include "../Adollib/Scripts/Imgui/work_meter.h"

#include "../Adollib/Scripts/Main/systems.h"

#include "../Adollib/Imgui/imgui.h"
#include "../../Scripts/Main/mono_audio.h"

using namespace Adollib;
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

	MonoAudio::PlayMusic(Music::DEMO, true);
}

void Demo_music::update() {
	ImGuiWindowFlags flag = 0;
	//flag |= ImGuiWindowFlags_AlwaysAutoResize;
	flag |= ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysAutoResize;
	flag |= ImGuiWindowFlags_::ImGuiWindowFlags_NoDocking;
	if (ImGui::Begin("Music_demo", 0, flag)) {

		// reserve_type
		{
			static int R = 0;
			ImGui::InputInt("Reserve", &R, 1);
			if (R < 0)R = 0;
			if (DirectX::AUDIO_ENGINE_REVERB::Reverb_MAX <= R)R = DirectX::AUDIO_ENGINE_REVERB::Reverb_MAX - 1;
			MonoAudio::audioEngine->SetReverb(static_cast<DirectX::AUDIO_ENGINE_REVERB>(R));
		}

		// listener_pos
		{
			static Vector3 pos;
			ImGui::DragFloat3("my_pos", &pos.x, 0.1f, -10, 10);
			MonoAudio::listener.SetPosition(pos);
		}

		// sound_pos
		{
			static Vector3 pos;
			ImGui::DragFloat3("sound_pos", &pos.x, 0.1f, -10, 10);
			MonoAudio::emitter.SetPosition(pos);
		}

		MonoAudio::musicData.musicInst[Music::DEMO]->Apply3D(MonoAudio::listener, MonoAudio::emitter);

	}
	ImGui::End();

}
