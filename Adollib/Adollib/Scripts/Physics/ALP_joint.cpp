#include "ALP_joint.h"

#include "joint_base.h"
#include "ALP__physics_manager.h"

using namespace Adollib;
using namespace Physics_function;

void ALP_Joint::reset() {
	bias = 1;
	constraint[0].accuminpulse = 0;
	constraint[1].accuminpulse = 0;
	constraint[2].accuminpulse = 0;
}

void ALP_Joint::adapt_Jointdata() {


	joint->adapt_anchor();

	anchor = joint->anchors;
	anchor_count = joint->anchor_count;
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