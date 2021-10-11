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



		//::: ALP_phyicses ::::::::::

		// ���x�A�����x��0�ɂ���
		void resetforce(std::list<Physics_function::ALP_Physics*>& ALP_physics);

		// �O�͂ɂ�鑬�x�Ȃǂ̍X�V
		void applyexternalforce(std::list<Physics_function::ALP_Physics*>& ALP_physics, const float timeratio_60);

		// ���W,�p���̍X�V
		void integrate(std::list<Physics_function::ALP_Physics*>& ALP_physics);


		//::: �ǂ����� ::::
		// oncoll_bits��trnsform�̃��Z�b�g
		void reset_data_per_frame(std::list<Physics_function::ALP_Collider*>& ALP_colliders, std::list<Physics_function::ALP_Physics*>& ALP_physics);

		// collider::component��tag��physics_data���X�V����
		void adapt_collider_component_data(std::list<Physics_function::ALP_Collider*>& ALP_colliders, std::list<Physics_function::ALP_Physics*>& ALP_physics);

		//void adapt_added_data(std::list<Physics_function::ALP_Collider*>& ALP_colliders, std::list<Physics_function::ALP_Physics*>& ALP_physics, std::list<Physics_function::ALP_Joint*>& ALP_joints);

	}
}