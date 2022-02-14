#include "stage_manager.h"

#include "../Adollib/Scripts/Main/Adollib.h"

#include "stageparts_base.h"
#include "stage_base.h"

#include "stage_demo.h"
#include "stage_demo_1.h"
#include "stage_demo_2.h"
#include "stage_01.h"
#include "stage_02.h"

#include "player.h"

using namespace Adollib;
using namespace Stage_parts;

void Stage_manager::awake() {

	add_stage<Stage_demo>(Stage_types::demo);
	add_stage<Stage_demo_01>(Stage_types::demo_1);
	add_stage<Stage_demo_02>(Stage_types::demo_2);

	add_stage<Stage_01>(Stage_types::stage_1);
	add_stage<Stage_02>(Stage_types::stage_2);
	add_stage<Stage_demo>(Stage_types::stage_3);

	now_stage = Stage_types::none; //¡‚Ìstagetype
	next_stage = Stage_types::none; //ŽŸ‚Ìstagetype

	//stages[now_stage]->stage_awake();
}

void Stage_manager::start() {

	player = Gameobject_manager::find("player",Scenelist::scene_player)->findComponent<Player>();
	player->respown();
}

void Stage_manager::update() {
	if (Al_Global::second_per_game < 2)return;
	stage_timer += time->deltaTime();
	is_changed_stage = false;

	if (now_stage != next_stage) {
		stage_timer = 0;

		if (now_stage != Stage_types::none) stages[now_stage]->stage_destroy();
		if (next_stage != Stage_types::none) stages[next_stage]->stage_awake();

		now_stage = next_stage;

		//player->delete_catchjoint();
		player->respown();

		is_changed_stage = true;
	}

	if (input->getKeyTrigger(Key::F1))
		set_next_stage_type(Stage_types::stage_1);
	if (input->getKeyTrigger(Key::F2))
		set_next_stage_type(Stage_types::stage_2);
	if (input->getKeyTrigger(Key::F3))
		set_next_stage_type(Stage_types::demo_1);
	if (input->getKeyTrigger(Key::F4))
		set_next_stage_type(Stage_types::demo_2);


	if (input->getKeyTrigger(Key::F11) || input->getPadTrigger(0, GamePad::BACK)) {
		Scene_manager::set_inactive(Scenelist::scene_game);
		Scene_manager::set_inactive(Scenelist::scene_player);
		Scene_manager::set_active(Scenelist::scene_title);
	}
}

void Stage_manager::set_next_stage_type(Stage_types next_stge) {
	next_stage = next_stge;
}