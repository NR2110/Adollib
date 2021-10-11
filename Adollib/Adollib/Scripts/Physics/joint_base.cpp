#include "joint_base.h"

#include "ALP_joint.h"
#include "ALP__physics_manager.h"

using namespace Adollib;
using namespace Physics_function;

void Joint_base::destroy() {

	Phyisics_manager::add_delete_buffer_ALPJoint(ALPjoint);
	//ALPjoint->destroy(nullptr, false);
	//delete ALPjoint;

}