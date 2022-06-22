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
	// physics_manager����폜
	Physics_manager::remove_Joint(this_itr);

	// coll_ptr�̑����̕ێ�����joint�z�񂩂玩�g���폜
	// coll_ptr��null�łȂ��Ƃ� coll_ptr���ێ�����joints����destroy���Ă�ł��邽�� remove_joint����Ǝ���
	if (ALPcollider[0] != coll_ptr) ALPcollider[0]->remove_joint(this);
	if (ALPcollider[1] != coll_ptr) ALPcollider[1]->remove_joint(this);

	// user�p�ɏ������Ă������̂��폜
	delete userjoint;
	userjoint = nullptr;

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

void ALP_Joint::set_ptr_to_joint(ALP_Joint* joint) {
	userjoint->get_colliderA()->set_ptr_to_joint(joint);
	userjoint->get_colliderB()->set_ptr_to_joint(joint);
}