#include "stage_manager.h"

#include "stageparts_base.h"
#include "stage_base.h"

#include "stage_demo.h"

#include "player.h"

using namespace Adollib;
using namespace Stage_parts;

void Stage_manager::awake() {

	add_stage<Stage_demo>(Stage_types::demo);
	add_stage<Stage_demo>(Stage_types::stage_0);

	stages[now_stage]->stage_awake();

}

void Stage_manager::start() {

}

void Stage_manager::update() {
	if (now_stage != next_stage) {

		stages[now_stage]->stage_destroy();
		stages[next_stage]->stage_awake();

		now_stage = next_stage;
	}
}

void Stage_manager::set_next_stage_type(Stage_types next_stge) {
	next_stage = next_stge;
}