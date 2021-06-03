#pragma once
#include <memory>
#include "../Scene/scene.h"
#include "ALP_collider.h"
#include "collider_shape.h"


namespace Adollib {
	namespace Physics_function {


		//::: ALP_colliders ::::::::::

		// gameobject��transform����collider��world��Ԃł̏����X�V
		void update_world_trans(std::list<Physics_function::ALP_Collider*>& ALP_colliders);

		// gameobject�ւ̕ω��ʂ����߂�
		void solv_resolve(std::list<Physics_function::ALP_Collider*>& ALP_colliders);

		// gameobject�֕ω��ʂ�n��
		void resolve_gameobject(std::list<Physics_function::ALP_Collider*>& ALP_colliders);

		// k-DOP�̍X�V
		void update_dop14(std::list<Physics_function::ALP_Collider*>& ALP_shapes);

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