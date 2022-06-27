#include "joint.h"
#include "ALP__physics_manager.h"
#include "collider.h"

#include "balljoint.h"
#include "hingejoint.h"
#include "twistjoint.h"

using namespace Adollib;
using namespace Physics_function;
//#pragma optimize("", off)
BallJoint* Joint::add_balljoint(
	std::shared_ptr<Collider> colliderA, std::shared_ptr<Collider> colliderB,
	const Vector3& anchorA, const Vector3& anchorB,
	const float& bias
) {
	//joint�̐���
	auto joint = Physics_manager::add_Joint();
	//colliderA->set_ptr_to_joint(joint);
	//colliderB->set_ptr_to_joint(joint);

	BallJoint* balljoint = newD BallJoint(colliderA, colliderB, joint);
	joint->userjoint = balljoint;
	joint->set_ptr_to_joint(joint);

	balljoint->anchor.posA = anchorA;
	balljoint->anchor.posB = anchorB;
	balljoint->shrink_bias = bias;
	balljoint->stretch_bias = bias;

	return balljoint;
}
//#pragma optimize("", on)

HingeJoint* Joint::add_Hingejoint(
	std::shared_ptr<Collider> colliderA, std::shared_ptr<Collider> colliderB,
	const Vector3& anchorA_s, const Vector3& anchorA_g,
	const Vector3& anchorB_s, const Vector3& anchorB_g,
	const float& bias,
	const float& hingepow
) {
	//joint�̐���
	auto joint = Physics_manager::add_Joint();
	//colliderA->set_ptr_to_joint(joint);
	//colliderB->set_ptr_to_joint(joint);

	HingeJoint* hingejoint = newD HingeJoint(colliderA, colliderB, joint);
	joint->userjoint = hingejoint;
	joint->set_ptr_to_joint(joint);

	colliderA->physics_data.drag = 0.8f;
	colliderB->physics_data.drag = 0.8f;

	hingejoint->anchor_s.posA = anchorA_s;
	hingejoint->anchor_s.posB = anchorB_s;

	hingejoint->anchor_g.posA = anchorA_g;
	hingejoint->anchor_g.posB = anchorB_g;

	hingejoint->shrink_bias = bias;
	hingejoint->stretch_bias = bias;
	hingejoint->hinge_pow = hingepow;
	return hingejoint;
}

ConeJoint* Joint::add_Conejoint(
	std::shared_ptr<Collider> colliderA, std::shared_ptr<Collider> colliderB,
	const Vector3& anchorA, const Vector3& anchorB,
	const Vector3& axisA, const Vector3& axisB,
	const float& bias
) {
	//joint�̐���
	auto joint = Physics_manager::add_Joint();
	//colliderA->set_ptr_to_joint(joint);
	//colliderB->set_ptr_to_joint(joint);

	ConeJoint* conetwist = newD ConeJoint(colliderA, colliderB, joint);
	joint->userjoint = conetwist;
	joint->set_ptr_to_joint(joint);

	conetwist->anchor.posA = anchorA;
	conetwist->anchor.posB = anchorB;

	conetwist->limit_axis[0] = axisA;
	conetwist->limit_axis[1] = axisB;

	conetwist->shrink_bias = bias;
	conetwist->stretch_bias = bias;

	return conetwist;
}

ConetwistJoint* Joint::add_Conetwistjoint(
	std::shared_ptr<Collider> colliderA, std::shared_ptr<Collider> colliderB,
	const Vector3& anchorA, const Vector3& anchorB,
	const Vector3& axisA, const Vector3& axisB,
	const float& bias
) {
	//joint�̐���
	auto joint = Physics_manager::add_Joint();
	//colliderA->set_ptr_to_joint(joint);
	//colliderB->set_ptr_to_joint(joint);

	ConetwistJoint* conetwist = newD ConetwistJoint(colliderA, colliderB, joint);
	joint->userjoint = conetwist;
	joint->set_ptr_to_joint(joint);

	conetwist->anchor.posA = anchorA;
	conetwist->anchor.posB = anchorB;

	conetwist->limit_axis[0] = axisA;
	conetwist->limit_axis[1] = axisB;

	conetwist->shrink_bias = bias;
	conetwist->stretch_bias = bias;

	return conetwist;
}

TwistJoint* Joint::add_Twistjoint(
	std::shared_ptr<Collider> colliderA, std::shared_ptr<Collider> colliderB,
	const Vector3& axisA,
	const Vector3& axisB,
	const float& bias
) {
	//joint�̐���
	auto joint = Physics_manager::add_Joint();
	//colliderA->set_ptr_to_joint(joint);
	//colliderB->set_ptr_to_joint(joint);

	TwistJoint* twistjoint = newD TwistJoint(colliderA, colliderB, joint);
	joint->userjoint = twistjoint;
	joint->set_ptr_to_joint(joint);

	twistjoint->vec0 = axisA;
	twistjoint->vec1 = axisB;

	twistjoint->shrink_bias = bias;
	twistjoint->stretch_bias = bias;

	return twistjoint;
}




void Joint::delete_joint(Joint_base*& joint) {
	if (joint == nullptr)return;
	joint->destroy();
	joint = nullptr;
}

