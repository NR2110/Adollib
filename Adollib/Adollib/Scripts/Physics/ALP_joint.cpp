#include "ALP_joint.h"

#include "joint_base.h"
#include "ALP__physics_manager.h"

using namespace Adollib;
using namespace Physics_function;

void ALP_Joint::reset() {
	bias = 1;
	constraint.accuminpulse = 0;
}

void ALP_Joint::adapt_Jointdata() {
	anchor[0] = joint->anchor[0];
	anchor[1] = joint->anchor[1];
	bias = joint->bias;
}


void ALP_Joint::destroy(ALP_Collider* coll_ptr, const bool is_delete_userjoint) {
	Phyisics_manager::remove_Joint(this_itr);

	if (ALPcollider[0] != coll_ptr) ALPcollider[0]->remove_joint(this);
	if (ALPcollider[1] != coll_ptr) ALPcollider[1]->remove_joint(this);

	if (is_delete_userjoint == true) {
		delete joint;
	}
}