#pragma once
#include "contact.h"
#include "collider_shape.h"

#include <memory>
namespace Adollib {
	namespace Physics_function {

		//�Փˉ���
		void resolve_contact(std::list<ALP_Collider*>& colliders, std::vector<Contacts::Contact_pair>& pairs);

	}
}