#include "area_respown_set.h"


using namespace Adollib;
using namespace Stage_parts;

void Area_respown_set::awake() {
}


void Area_respown_set::update() {
	if (this_coll->concoll_enter(Collider_tags::Human)) {
		if (this_stage->respown_num < respown_num)
			this_stage->player_respown_pos = respown_position;

		this_stage->respown_num = respown_num;
	}
}
