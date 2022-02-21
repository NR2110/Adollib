#include "scene_player.h"
#include "../Object/gameobject_manager.h"

#include "../../../Scripts/scene_game_all.h"

#include "../../Scripts/Main/mono_audio.h"
namespace Adollib {


	void Scene_player::initialize() {

		//ALKLib::MonoAudio::PlayMusic(0, true);
		//ALKLib::MonoAudio::PlayMusic(1, true);

		Gameobject* Player = Gameobject_manager::create("Player_manager");
		Player->addComponent<Player_manager>();

	}



}