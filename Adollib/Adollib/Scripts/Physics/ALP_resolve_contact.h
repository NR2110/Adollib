#pragma once
#include "contact.h"
#include <memory>
namespace Adollib {
	namespace physics_function {

		//�Փˉ���
		void resolve_contact(std::list<ALP_Collider>& colliders, std::vector<Contacts::Contact_pair>& pairs);

	}
}