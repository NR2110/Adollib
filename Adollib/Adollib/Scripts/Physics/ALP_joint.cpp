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
	// physics_manager����폜
	Phyisics_manager::remove_Joint(this_itr);

	// coll_ptr�̑����̕ێ�����joint�z�񂩂玩�g���폜
	if (ALPcollider[0] != coll_ptr) ALPcollider[0]->remove_joint(this);
	if (ALPcollider[1] != coll_ptr) ALPcollider[1]->remove_joint(this);

	// ALPCollider����Ă΂ꂽ�ꍇ��userjoint���폜
	if (is_delete_userjoint == true) {
		delete userjoint;
	}

	//::::::::::::
	// Joint::destroy�� "ALPCollider�̍폜" "userjoint�̍폜" ��2������Ă΂��\��������
	// "ALPCollider�̍폜" �ł�
	//  ������joint�z�񂩂玩�g�̍폜,
	//  �Ή�����userjointnt�̍폜
	// ���s��
	// "userjoint�̍폜" �ł�
	//  �o����joint�z�񂩂玩�g�̂��폜���s��
	// userjoint���炱�̊֐����Ă΂�邽�� �����ł�userjoint��delete�ł��Ȃ�
	//::::::::::::::
}