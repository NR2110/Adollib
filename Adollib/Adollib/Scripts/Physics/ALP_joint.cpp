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

	userjoint->adapt_anchor();

	anchor = userjoint->get_anchors();
	anchor_count = userjoint->get_anchors_count();
	bias = userjoint->bias;
	slop = userjoint->slop;
	limit_bias = userjoint->limit_bias;
}

void ALP_Joint::destroy(ALP_Collider* coll_ptr, const bool is_delete_userjoint) {
	// physics_managerから削除
	Phyisics_manager::remove_Joint(this_itr);

	// coll_ptrの相方の保持するjoint配列から自身を削除
	if (ALPcollider[0] != coll_ptr) ALPcollider[0]->remove_joint(this);
	if (ALPcollider[1] != coll_ptr) ALPcollider[1]->remove_joint(this);

	// ALPColliderから呼ばれた場合はuserjointを削除
	if (is_delete_userjoint == true) {
		delete userjoint;
	}

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