#include "joint_base.h"

#include "ALP_joint.h"
#include "ALP__physics_manager.h"

using namespace Adollib;
using namespace Physics_function;

void Joint_base::destroy() {
	ALPjoint->destroy();
	delete ALPjoint;
}