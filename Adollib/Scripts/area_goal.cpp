#include "area_goal.h"

#include "stage_manager.h"

using namespace Adollib;
using namespace Stage_parts;

void Area_goal::awake() {
	stage_manager = Gameobject_manager::find("Stage_manager")->findComponent<Stage_manager>();
}


void Area_goal::update() {
	if (this_coll->concoll_enter(Collider_tags::Human)) {
		this_stage->next_stage = next_stage;
	}
}
