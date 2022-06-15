
#include "demo_music.h"
#include <unordered_map>

#include "../Adollib/Scripts/Object/gameobject_manager.h"

#include "../Adollib/Scripts/Renderer/renderer_base.h"

#include "../Adollib/Scripts/Imgui/work_meter.h"

#include "../Adollib/Scripts/Main/systems.h"

#include "../Adollib/Imgui/imgui.h"
#include "../../Scripts/Main/mono_audio.h"


#include "../../Scripts/Audio/audio_manager.h"

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

void Demo_music::start() {

	audio->load("../Data/sounds/Demo_BGM/Theme_of_Rabi-Ribi.wav", true);
	audio->play(true);
}

void Demo_music::update() {


}
