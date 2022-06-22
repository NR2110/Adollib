#include "ALP_joint.h"

#include "joint_base.h"
#include "ALP__physics_manager.h"

using namespace Adollib;
using namespace Physics_function;

void ALP_Joint::reset() {
	shrink_bias = 1;
	stretch_bias = 1;
	constraint[0].accuminpulse = 0;
	constraint[1].accuminpulse = 0;
	constraint[2].accuminpulse = 0;
}

void ALP_Joint::adapt_Jointdata() {
	userjoint->adapt_anchor();

	anchor = userjoint->get_anchors();
	anchor_count = userjoint->get_anchors_count();
	shrink_bias = userjoint->shrink_bias;
	stretch_bias = userjoint->stretch_bias;
	slop = userjoint->slop;
	offset = userjoint->offset;
	limit_bias = userjoint->limit_bias;
}

void ALP_Joint::apply_joint_velocityeffect() {

	userjoint->velocity_effect();
}


void ALP_Joint::destroy(ALP_Collider* coll_ptr) {
	// physics_managerから削除
	Physics_manager::remove_Joint(this_itr);

	// coll_ptrの相方の保持するjoint配列から自身を削除
	// coll_ptrがnullでないとき coll_ptrが保持するjointsからdestroyを呼んでいるため remove_jointすると死ぬ
	if (ALPcollider[0] != coll_ptr) ALPcollider[0]->remove_joint(this);
	if (ALPcollider[1] != coll_ptr) ALPcollider[1]->remove_joint(this);

	// user用に準備していた実体を削除
	delete userjoint;
	userjoint = nullptr;

	//::::::::::::
	// Joint::destroyは "ALPColliderの削除" "userjointの削除" の2個所から呼ばれる可能性がある
	// "ALPColliderの削除" では
	//  相方のjoint配列から自身の削除,
	//  対応するuserjointntの削除
	// を行う
	// "userjointの削除" では
	//  双方のjoint配列から自身のを削除を行う
	// userjointからこの関数が呼ばれるため ここではuserjointをdeleteできない
	//::::::::::::::
}

void ALP_Joint::set_ptr_to_joint(ALP_Joint* joint) {
	userjoint->get_colliderA()->set_ptr_to_joint(joint);
	userjoint->get_colliderB()->set_ptr_to_joint(joint);
}