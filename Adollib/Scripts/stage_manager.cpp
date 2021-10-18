#include "stage_manager.h"

#include "../Adollib/Scripts/Main/Adollib.h"

#include "stageparts_base.h"
#include "stage_base.h"

#include "stage_demo.h"
#include "stage_01.h"

#include "player.h"

using namespace Adollib;
using namespace Stage_parts;

void Stage_manager::awake() {

	add_stage<Stage_demo>(Stage_types::demo);
	add_stage<Stage_01>(Stage_types::stage_0);
	//add_stage<Stage_01>(Stage_types::stage_1);

	now_stage = Stage_types::stage_0; //¡‚Ìstagetype
	next_stage = Stage_types::stage_0; //ŽŸ‚Ìstagetype

	stages[now_stage]->stage_awake();

}

void Stage_manager::start() {
	player = Gameobject_manager::find("player")->findComponent<Player>();
	//player->respown();
}

void Stage_manager::update() {
	if (Al_Global::second_per_game < 2)return;
	is_changed_stage = false;

	if (now_stage != next_stage) {

		stages[now_stage]->stage_destroy();
		stages[next_stage]->stage_awake();

		now_stage = next_stage;

		player->delete_catchjoint();
		player->respown();

		is_changed_stage = true;
	}

	if (input->getKeyTrigger(Key::R)) player->respown();

	if (input->getKeyTrigger(Key::D1))
		set_next_stage_type(Stage_types::demo);
	if (input->getKeyTrigger(Key::D2))
		set_next_stage_type(Stage_types::stage_0);
}

void Stage_manager::set_next_stage_type(Stage_types next_stge) {
	next_stage = next_stge;
}