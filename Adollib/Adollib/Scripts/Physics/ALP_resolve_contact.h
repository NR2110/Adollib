#pragma once
#include "ALP_contact.h"
#include "collider_shape.h"
#include "ALP_joint.h"

#include <memory>
namespace Adollib {
	namespace Physics_function {

		//�Փˉ���
		void resolve_contact(
			std::list<Physics_function::ALP_Collider*>& ALP_colliders,
			std::vector<Contacts::Contact_pair*>& pairs,
			std::list<Physics_function::ALP_Joint*> joints,
			const float time_scale);

	}
}