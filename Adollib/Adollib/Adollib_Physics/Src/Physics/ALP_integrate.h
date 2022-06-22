#pragma once
#include <list>
#include <unordered_map>

namespace Adollib {
	namespace Physics_function {
		class ALP_Joint;
		class ALP_Collider;
		class ALP_Physics;

		//::: ALP_Joint ::::::::
		void apply_joint_velocityeffect(std::list<Physics_function::ALP_Joint*>& ALP_Joints);


		//::: ALP_colliders ::::::::::
		// gameobject��transform����collider��world��Ԃł̏����X�V
		void update_world_trans(std::list<Physics_function::ALP_Collider*>& ALP_colliders);

		// calculate���s���邽�эX�V�������
		void update_per_calculate(std::list<Physics_function::ALP_Collider*>& ALP_colliders);



		//::: ALP_phyicses ::::::::::

		// ���x�A�����x��0�ɂ���
		void resetforce(std::list<Physics_function::ALP_Physics*>& ALP_physics);

		// �O�͂ɂ�鑬�x�Ȃǂ̍X�V
		void applyexternalforce(
			std::list<Physics_function::ALP_Physics*>& ALP_physics,
			const float timeratio_60, const float time_scale
		);

		// ���W,�p���̍X�V
		void integrate(std::list<Physics_function::ALP_Physics*>& ALP_physics);


		//::: �ǂ����� ::::
		// gameobject���X�V����world_trnsform�̍X�V
		void copy_transform(
			std::list<Physics_function::ALP_Collider*>& ALP_colliders,
			std::list<Physics_function::ALP_Physics*>& ALP_physics
		);

		// collider::component��tag��physics_data���X�V����
		void adapt_component_data(
			std::list<Physics_function::ALP_Collider*>& ALP_colliders,
			std::list<Physics_function::ALP_Physics*>& ALP_physics,
			std::list<Physics_function::ALP_Joint*>& ALP_joints
		);

		//void adapt_added_data(std::list<Physics_function::ALP_Collider*>& ALP_colliders, std::list<Physics_function::ALP_Physics*>& ALP_physics, std::list<Physics_function::ALP_Joint*>& ALP_joints);

	}
}