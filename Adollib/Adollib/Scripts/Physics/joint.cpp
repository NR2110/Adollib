#include "joint.h"
#include "ALP__physics_manager.h"
#include "collider.h"

#include "balljoint.h"
#include "hingejoint.h"

using namespace Adollib;
using namespace Physics_function;

BallJoint* Joint::add_balljoint(
	Collider* colliderA, Collider* colliderB,
	const Vector3& anchorA, const Vector3& anchorB,
	const float& bias
) {
	//jointの生成
	auto joint = Phyisics_manager::add_Joint();
	colliderA->set_ptr_to_joint(joint);
	colliderB->set_ptr_to_joint(joint);

	BallJoint* balljoint = newD BallJoint(colliderA, colliderB, joint);
	joint->joint = balljoint;

	balljoint->anchor.posA = anchorA;
	balljoint->anchor.posB = anchorB;
	balljoint->bias = bias;

	return balljoint;
}

HingeJoint* Joint::add_Hingejoint(
	Collider* colliderA, Collider* colliderB,
	const Vector3& anchorA_s, const Vector3& anchorA_g,
	const Vector3& anchorB_s, const Vector3& anchorB_g,
	const float& hingepow ,
	const float& bias
) {
	//jointの生成
	auto joint = Phyisics_manager::add_Joint();
	colliderA->set_ptr_to_joint(joint);
	colliderB->set_ptr_to_joint(joint);

	HingeJoint* hingejoint = newD HingeJoint(colliderA, colliderB, joint);
	joint->joint = hingejoint;

	colliderA->physics_data.drag = 0.8f;
	colliderB->physics_data.drag = 0.8f;

	hingejoint->anchor_s.posA = anchorA_s;
	hingejoint->anchor_s.posB = anchorB_s;

	hingejoint->anchor_g.posA = anchorA_g;
	hingejoint->anchor_g.posB = anchorB_g;

	hingejoint->bias = bias;
	hingejoint->hinge_pow = hingepow;
	return hingejoint;
}

ConetwistJoint* Joint::add_Conetwistjoint(
	Collider* colliderA, Collider* colliderB,
	const Vector3& anchorA, const Vector3& anchorB,
	const Vector3& axisA, const Vector3& axisB,
	const float& bias
) {
	//jointの生成
	auto joint = Phyisics_manager::add_Joint();
	colliderA->set_ptr_to_joint(joint);
	colliderB->set_ptr_to_joint(joint);

	ConetwistJoint* conetwist = newD ConetwistJoint(colliderA, colliderB, joint);
	joint->joint = conetwist;

	conetwist->anchor.posA = anchorA;
	conetwist->anchor.posB = anchorB;

	conetwist->limit_axis[0] = axisA;
	conetwist->limit_axis[1] = axisA;

	conetwist->bias = bias;

	return conetwist;
}


void Joint::delete_joint(Joint_base*& joint) {
	joint->destroy();
	delete joint;
	joint = nullptr;
}

