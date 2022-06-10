#pragma once

#include "../../DirectXTK-master/Inc/Audio.h"

class Audio_reverb {

public:
	// ratio of wet (processed) signal to dry (original) signal
	float WetDryMix;            // [0, 100] (percentage)

	// Standard I3DL2 parameters
	int Room;                 // [-10000, 0] in mB (hundredths of decibels)
	int RoomHF;               // [-10000, 0] in mB (hundredths of decibels)
	float RoomRolloffFactor;    // [0.0, 10.0]
	float DecayTime;            // [0.1, 20.0] in seconds
	float DecayHFRatio;         // [0.1, 2.0]
	int Reflections;          // [-10000, 1000] in mB (hundredths of decibels)
	float ReflectionsDelay;     // [0.0, 0.3] in seconds
	int Reverb;               // [-10000, 2000] in mB (hundredths of decibels)
	float ReverbDelay;          // [0.0, 0.1] in seconds
	float Diffusion;            // [0.0, 100.0] (percentage)
	float Density;              // [0.0, 100.0] (percentage)
	float HFReference;          // [20.0, 20000.0] in Hz




public:
	enum class Reverb_effects_enum {
		Reverb_Default,
		Reverb_Forest,
		Reverb_PaddedCell,
		Reverb_Room,
		Reverb_Bathroom,
		Reverb_LivingRoom,
		Reverb_StoneRoom,
		Reverb_Auditorium,
		Reverb_ConcertHall,
		Reverb_Cave,
		Reverb_Arena,
		Reverb_Hangar,
		Reverb_CarpetedHallway,
		Reverb_Hallway,
		Reverb_StoneCorridor,
		Reverb_Alley,
		Reverb_City,
		Reverb_Mountains,
		Reverb_Quarry,
		Reverb_Plain,
		Reverb_ParkingLot,
		Reverb_SewerPipe,
		Reverb_Underwater,
		Reverb_SmallRoom,
		Reverb_MediumRoom,
		Reverb_LargeRoom,
		Reverb_MediumHall,
		Reverb_LargeHall,
		Reverb_Plate,
		Reverb_Max
	};
	void setparam_from_prepared(Reverb_effects_enum name);
	void set_reverb();


};