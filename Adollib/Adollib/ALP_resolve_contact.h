#pragma once
#include "compute_shader.h"
#include "contact.h"

namespace Adollib {
	namespace physics_function {
		struct Solverbody {
			vector3 delta_LinearVelocity; // ���i���x����
			vector3 delta_AngularVelocity; // ��]���x����
			quaternion orientation; // �p��
			matrix inv_inertia; // �����e���\���̋t�s��
			float  inv_mass; // ���ʂ̋t��
			int num;
		};

		//�Փˉ���
		void resolve_contact(std::vector<Collider*> colliders, std::vector<Contacts::Contact_pair>& pairs);

	}
}