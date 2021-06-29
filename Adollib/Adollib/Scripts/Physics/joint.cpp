#include "joint.h"
#include "ALP__physics_manager.h"

#include "ALP_balljoint.h"

using namespace Adollib;
using namespace Physics_function;

Joint_data Joint::add_balljoint(
	Collider* colliderA, Vector3 anchorA,
	Collider* colliderB, Vector3 anchorB,
	float bias
) {

	auto joint = Phyisics_manager::add_Joint<ALP_BallJoint>();

	Joint_data ret(joint);

	return ret;
}