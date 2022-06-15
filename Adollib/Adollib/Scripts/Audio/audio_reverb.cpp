//
//#include "audio_reverb.h"
//
//#include "../../DirectXTK-master/Inc/Audio.h"
//#include "audio_manager.h"
//
//using namespace Adollib;
//
//static const XAUDIO2FX_REVERB_I3DL2_PARAMETERS gReverbPresets[] =
//{
//	XAUDIO2FX_I3DL2_PRESET_DEFAULT,             // Reverb_Default
//	XAUDIO2FX_I3DL2_PRESET_FOREST,              // Reverb_Forest
//	XAUDIO2FX_I3DL2_PRESET_PADDEDCELL,          // Reverb_PaddedCell
//	XAUDIO2FX_I3DL2_PRESET_ROOM,                // Reverb_Room
//	XAUDIO2FX_I3DL2_PRESET_BATHROOM,            // Reverb_Bathroom
//	XAUDIO2FX_I3DL2_PRESET_LIVINGROOM,          // Reverb_LivingRoom
//	XAUDIO2FX_I3DL2_PRESET_STONEROOM,           // Reverb_StoneRoom
//	XAUDIO2FX_I3DL2_PRESET_AUDITORIUM,          // Reverb_Auditorium
//	XAUDIO2FX_I3DL2_PRESET_CONCERTHALL,         // Reverb_ConcertHall
//	XAUDIO2FX_I3DL2_PRESET_CAVE,                // Reverb_Cave
//	XAUDIO2FX_I3DL2_PRESET_ARENA,               // Reverb_Arena
//	XAUDIO2FX_I3DL2_PRESET_HANGAR,              // Reverb_Hangar
//	XAUDIO2FX_I3DL2_PRESET_CARPETEDHALLWAY,     // Reverb_CarpetedHallway
//	XAUDIO2FX_I3DL2_PRESET_HALLWAY,             // Reverb_Hallway
//	XAUDIO2FX_I3DL2_PRESET_STONECORRIDOR,       // Reverb_StoneCorridor
//	XAUDIO2FX_I3DL2_PRESET_ALLEY,               // Reverb_Alley
//	XAUDIO2FX_I3DL2_PRESET_CITY,                // Reverb_City
//	XAUDIO2FX_I3DL2_PRESET_MOUNTAINS,           // Reverb_Mountains
//	XAUDIO2FX_I3DL2_PRESET_QUARRY,              // Reverb_Quarry
//	XAUDIO2FX_I3DL2_PRESET_PLAIN,               // Reverb_Plain
//	XAUDIO2FX_I3DL2_PRESET_PARKINGLOT,          // Reverb_ParkingLot
//	XAUDIO2FX_I3DL2_PRESET_SEWERPIPE,           // Reverb_SewerPipe
//	XAUDIO2FX_I3DL2_PRESET_UNDERWATER,          // Reverb_Underwater
//	XAUDIO2FX_I3DL2_PRESET_SMALLROOM,           // Reverb_SmallRoom
//	XAUDIO2FX_I3DL2_PRESET_MEDIUMROOM,          // Reverb_MediumRoom
//	XAUDIO2FX_I3DL2_PRESET_LARGEROOM,           // Reverb_LargeRoom
//	XAUDIO2FX_I3DL2_PRESET_MEDIUMHALL,          // Reverb_MediumHall
//	XAUDIO2FX_I3DL2_PRESET_LARGEHALL,           // Reverb_LargeHall
//	XAUDIO2FX_I3DL2_PRESET_PLATE,               // Reverb_Plate
//};
//
//void Audio_reverb::set_reverb(const Audio_reverb* reverb) {
//	if (reverb == nullptr) {
//		Audio_manager::set_reverb(nullptr);
//		return;
//	}
//
//	XAUDIO2FX_REVERB_I3DL2_PARAMETERS param;
//	param.WetDryMix         = reverb->WetDryMix;
//	param.Room              = reverb->Room;
//	param.RoomHF            = reverb->RoomHF;
//	param.RoomRolloffFactor = reverb->RoomRolloffFactor;
//	param.DecayTime         = reverb->DecayTime;
//	param.DecayHFRatio      = reverb->DecayHFRatio;
//	param.Reflections       = reverb->Reflections;
//	param.ReflectionsDelay  = reverb->ReflectionsDelay;
//	param.Reverb            = reverb->Reverb;
//	param.ReverbDelay       = reverb->ReverbDelay;
//	param.Diffusion         = reverb->Diffusion;
//	param.Density           = reverb->Density;
//	param.HFReference       = reverb->HFReference;
//
//	XAUDIO2FX_REVERB_PARAMETERS native;
//
//	ReverbConvertI3DL2ToNative(&param, &native);
//	Audio_manager::set_reverb(&native);
//}
//
//void Audio_reverb::setparam_from_prepared(Reverb_effects_enum name) {
//	auto param = gReverbPresets[static_cast<int>(name)];
//
//	WetDryMix         = param.WetDryMix;
//	Room              = param.Room;
//	RoomHF            = param.RoomHF;
//	RoomRolloffFactor = param.RoomRolloffFactor;
//	DecayTime         = param.DecayTime;
//	DecayHFRatio      = param.DecayHFRatio;
//	Reflections       = param.Reflections;
//	ReflectionsDelay  = param.ReflectionsDelay;
//	Reverb            = param.Reverb;
//	ReverbDelay       = param.ReverbDelay;
//	Diffusion         = param.Diffusion;
//	Density           = param.Density;
//	HFReference       = param.HFReference;
//}
//
