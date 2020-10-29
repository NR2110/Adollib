#pragma once
#include "compute_shader.h"
#include "contact.h"
#include <memory>
namespace Adollib {
	namespace physics_function {
		struct Solverbody {
			Vector3 delta_LinearVelocity; // ���i���x����
			Vector3 delta_AngulaVelocity; // ��]���x����
			Quaternion orientation; // �p��
			matrix inv_inertia; // �����e���\���̋t�s��
			float  inv_mass; // ���ʂ̋t��
			int num;
		};

		//�Փˉ���
		void resolve_contact(std::list<Adollib::Collider*>, std::vector<Contacts::Contact_pair>& pairs);

	}
}