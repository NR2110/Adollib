#include "joint.h"
#include "ALP__physics_manager.h"
#include "balljoint.h"

using namespace Adollib;
using namespace Physics_function;

BallJoint* Joint::add_balljoint(
	Collider* colliderA, Vector3 anchorA,
	Collider* colliderB, Vector3 anchorB,
	float bias
) {
	//joint‚Ì¶¬
	auto joint = Phyisics_manager::add_Joint();
	colliderA->set_ptr_to_joint(joint);
	colliderB->set_ptr_to_joint(joint);

	BallJoint* balljoint = newD BallJoint(colliderA, colliderB, joint);
	joint->joint = balljoint;

	balljoint->anchor[0] = anchorA;
	balljoint->anchor[1] = anchorB;
	balljoint->bias = bias;

	return balljoint;
}


void Joint::delete_joint(Joint_base*& joint) {
	joint->destroy();
	delete joint;
	joint = nullptr;
}

