#include "ALP_joint.h"

#include "joint_base.h"
#include "ALP__physics_manager.h"

using namespace Adollib;
using namespace Physics_function;

void ALP_Joint::adapt_Jointdata() {
	anchor[0] = joint->anchor[0];
	anchor[1] = joint->anchor[1];
	bias = joint->bias;
}


void ALP_Joint::destroy() {
	Phyisics_manager::remove_Joint(this_itr);
}