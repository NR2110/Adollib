#pragma once
#include "contact.h"
#include "ALP__shapes.h"

#include <memory>
namespace Adollib {
	namespace Physics_function {

		//�Փˉ���
		void resolve_contact(std::list<ALP_Collider*>& colliders, std::vector<Contacts::Contact_pair>& pairs);

	}
}