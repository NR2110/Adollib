#pragma once
#include <list>

namespace Adollib {
	namespace Physics_function {
		class ALP_Joint;
		class ALP_Collider;
		class ALP_Physics;

		//::: ALP_Joint ::::::::
		void adapt_joint_limit(std::list<Physics_function::ALP_Joint*>& ALP_Joints);


		//::: ALP_colliders ::::::::::
		// gameobject��transform����collider��world��Ԃł̏����X�V
		void update_world_trans(std::list<Physics_function::ALP_Collider*>& ALP_colliders);

		// k-DOP�̍X�V update_world_trans�ł����Ȃ��Ă���
		//void update_dop(std::list<Physics_function::ALP_Collider*>& ALP_shapes);

		// oncoll_bits�̃��Z�b�g
		void reset_oncoll_bits(std::list<Physics_function::ALP_Collider*>& ALP_colliders);


		//::: ALP_phyicses ::::::::::

		// ���x�A�����x��0�ɂ���
		void resetforce(std::list<Physics_function::ALP_Physics*>& ALP_physics);

		// �O�͂ɂ�鑬�x�Ȃǂ̍X�V
		void applyexternalforce(std::list<Physics_function::ALP_Physics*>& ALP_physics);

		// ���W,�p���̍X�V
		void integrate(std::list<Physics_function::ALP_Physics*>& ALP_physics);


	}
}