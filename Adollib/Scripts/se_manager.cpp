#include "se_manager.h"

#include "../Adollib/Scripts/Imgui/imgui_all.h"
#include "../Adollib/Scripts/Main/Adollib.h"
#include "../Adollib/Scripts/Imgui/debug.h"
#include "../../Scripts/Main/mono_audio.h"

using namespace Adollib;

void SE_manager::awake() {

}

void SE_manager::start() {

}

void SE_manager::update() {

	for (int i = 0; i < timer_size; ++i) {
		timer[i] -= time->deltaTime();
	}

}

void SE_manager::hand_pop_SE() {
	float& time = hand_pop_SE_timer;
	if(time < 0) ALKLib::MonoAudio::PlayMusic(ALKLib::Music::SE_Pop + player_num * 10, false);
	time = 0.5f;
}
void SE_manager::jump_SE() {
	float& time = jump_SE_timer;
	if (time < 0) ALKLib::MonoAudio::PlayMusic(ALKLib::Music::SE_Jump + player_num * 10, false);
	time = 0.5f;
}
void SE_manager::landing_SE() {
	float& time = landing_SE_timer;
	if (time < 0) ALKLib::MonoAudio::PlayMusic(ALKLib::Music::SE_Landing + player_num * 10, false);
	time = 0.5f;
}
void SE_manager::step_SE() {
	float& time = step_SE_timer;
	if (time < 0) {
		if(step_sign == -1) ALKLib::MonoAudio::PlayMusic(ALKLib::Music::SE_Step0+ player_num * 10, false);
		if(step_sign == +1) ALKLib::MonoAudio::PlayMusic(ALKLib::Music::SE_Step1+ player_num * 10, false);
	}
	if (time < 0) {
		time = 0.3f;
		step_sign *= -1;
	}
}



