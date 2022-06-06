
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

static const XAUDIO2FX_REVERB_I3DL2_PARAMETERS gReverbPresets[] =
{
	XAUDIO2FX_I3DL2_PRESET_DEFAULT,             // Reverb_Off
	XAUDIO2FX_I3DL2_PRESET_DEFAULT,             // Reverb_Default
	XAUDIO2FX_I3DL2_PRESET_GENERIC,             // Reverb_Generic
	XAUDIO2FX_I3DL2_PRESET_FOREST,              // Reverb_Forest
	XAUDIO2FX_I3DL2_PRESET_PADDEDCELL,          // Reverb_PaddedCell
	XAUDIO2FX_I3DL2_PRESET_ROOM,                // Reverb_Room
	XAUDIO2FX_I3DL2_PRESET_BATHROOM,            // Reverb_Bathroom
	XAUDIO2FX_I3DL2_PRESET_LIVINGROOM,          // Reverb_LivingRoom
	XAUDIO2FX_I3DL2_PRESET_STONEROOM,           // Reverb_StoneRoom
	XAUDIO2FX_I3DL2_PRESET_AUDITORIUM,          // Reverb_Auditorium
	XAUDIO2FX_I3DL2_PRESET_CONCERTHALL,         // Reverb_ConcertHall
	XAUDIO2FX_I3DL2_PRESET_CAVE,                // Reverb_Cave
	XAUDIO2FX_I3DL2_PRESET_ARENA,               // Reverb_Arena
	XAUDIO2FX_I3DL2_PRESET_HANGAR,              // Reverb_Hangar
	XAUDIO2FX_I3DL2_PRESET_CARPETEDHALLWAY,     // Reverb_CarpetedHallway
	XAUDIO2FX_I3DL2_PRESET_HALLWAY,             // Reverb_Hallway
	XAUDIO2FX_I3DL2_PRESET_STONECORRIDOR,       // Reverb_StoneCorridor
	XAUDIO2FX_I3DL2_PRESET_ALLEY,               // Reverb_Alley
	XAUDIO2FX_I3DL2_PRESET_CITY,                // Reverb_City
	XAUDIO2FX_I3DL2_PRESET_MOUNTAINS,           // Reverb_Mountains
	XAUDIO2FX_I3DL2_PRESET_QUARRY,              // Reverb_Quarry
	XAUDIO2FX_I3DL2_PRESET_PLAIN,               // Reverb_Plain
	XAUDIO2FX_I3DL2_PRESET_PARKINGLOT,          // Reverb_ParkingLot
	XAUDIO2FX_I3DL2_PRESET_SEWERPIPE,           // Reverb_SewerPipe
	XAUDIO2FX_I3DL2_PRESET_UNDERWATER,          // Reverb_Underwater
	XAUDIO2FX_I3DL2_PRESET_SMALLROOM,           // Reverb_SmallRoom
	XAUDIO2FX_I3DL2_PRESET_MEDIUMROOM,          // Reverb_MediumRoom
	XAUDIO2FX_I3DL2_PRESET_LARGEROOM,           // Reverb_LargeRoom
	XAUDIO2FX_I3DL2_PRESET_MEDIUMHALL,          // Reverb_MediumHall
	XAUDIO2FX_I3DL2_PRESET_LARGEHALL,           // Reverb_LargeHall
	XAUDIO2FX_I3DL2_PRESET_PLATE,               // Reverb_Plate
};

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

#ifdef _DEBUG
	eflags |= DirectX::AudioEngine_Debug;
#endif;

	audioEngine = std::make_unique<AudioEngine>(eflags);
	//audioEngine->SetReverb(AUDIO_ENGINE_REVERB::Reverb_Arena);
	//audioEngine->SetReverb(AUDIO_ENGINE_REVERB::Reverb_Default);

	soundeffect = new SoundEffect(audioEngine.get(), L"../Data/sounds/Demo_BGM/Theme_of_Rabi-Ribi.wav");
	audioInstance = soundeffect->CreateInstance(DirectX::SoundEffectInstance_ReverbUseFilters | DirectX::SoundEffectInstance_Use3D);
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

			//static int R = AUDIO_ENGINE_REVERB::Reverb_Default;
			//int ago = R;
			//ImGui::InputInt("Reserve", &R, 1);
			//if (R < 0)R = 0;
			//if (DirectX::AUDIO_ENGINE_REVERB::Reverb_MAX <= R)R = DirectX::AUDIO_ENGINE_REVERB::Reverb_MAX - 1;
			//DirectX::AUDIO_ENGINE_REVERB aaa = static_cast<DirectX::AUDIO_ENGINE_REVERB>(R);
			//audioEngine->SetReverb(aaa);

			//:::::::::::
			XAUDIO2FX_REVERB_PARAMETERS native;
			static XAUDIO2FX_REVERB_I3DL2_PARAMETERS param = gReverbPresets[Reverb_Default];

			static int R = AUDIO_ENGINE_REVERB::Reverb_Default;
			int ago = R;
			ImGui::InputInt("Reserve", &R, 1);
			if (R < 0)R = 0;
			if (DirectX::AUDIO_ENGINE_REVERB::Reverb_MAX <= R)R = DirectX::AUDIO_ENGINE_REVERB::Reverb_MAX - 1;
			if (R != ago)param = gReverbPresets[R];

			ImGui::SliderFloat("WetDryMix"        ,&param.WetDryMix,  0, 100);			// [0, 100] (percentage)
			ImGui::SliderInt  ("Room"             ,&param.Room, -10000, 0);		        // [-10000, 0] in mB (hundredths of decibels)
			ImGui::SliderInt  ("RoomHF"           ,&param.RoomHF, -10000, 0);		    // [-10000, 0] in mB (hundredths of decibels)
			ImGui::SliderFloat("RoomRolloffFactor",&param.RoomRolloffFactor, 0, 10);	// [0.0, 10.0]
			ImGui::SliderFloat("DecayTime"        ,&param.DecayTime, 0.1f, 20.0f);	    // [0.1, 20.0] in seconds
			ImGui::SliderFloat("DecayHFRatio"     ,&param.DecayHFRatio, 0.1f, 2.0f);	// [0.1, 2.0]
			ImGui::SliderInt  ("Reflections"      ,&param.Reflections, -10000, 1000);	// [-10000, 1000] in mB (hundredths of decibels)
			ImGui::SliderFloat("ReflectionsDelay" ,&param.ReflectionsDelay, 0, 0.3f);	// [0.0, 0.3] in seconds
			ImGui::SliderInt  ("Reverb"           ,&param.Reverb, -10000, 2000);		// [-10000, 2000] in mB (hundredths of decibels)
			ImGui::SliderFloat("ReverbDelay"      ,&param.ReverbDelay, 0.0f, 0.1f);     // [0.0, 0.1] in seconds
			ImGui::SliderFloat("Diffusion"        ,&param.Diffusion, 0, 100);			// [0.0, 100.0] (percentage)
			ImGui::SliderFloat("Density"          ,&param.Density, 0, 100);			    // [0.0, 100.0] (percentage)
			ImGui::SliderFloat("HFReference"      ,&param.HFReference, 20.0, 20000);	// [20.0, 20000.0] in Hz


			ReverbConvertI3DL2ToNative(&param, &native);
			audioEngine->SetReverb(&native);
			//::::::::::


			//static int R = AUDIO_ENGINE_REVERB::Reverb_Default;
			//int ago = R;
			//ImGui::InputInt("Reserve", &R, 1);
			//if (R < 0)R = 0;
			//if (DirectX::AUDIO_ENGINE_REVERB::Reverb_MAX <= R)R = DirectX::AUDIO_ENGINE_REVERB::Reverb_MAX - 1;
			//XAUDIO2FX_REVERB_PARAMETERS native;
			//ReverbConvertI3DL2ToNative(&gReverbPresets[R], &native);
			//audioEngine->SetReverb(&native);
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

	audioInstance->Apply3D(listener, emitter);

}
