#pragma once
#include "ALP_contact.h"
#include "collider_shape.h"
#include "ALP_joint.h"

#include <memory>
namespace Adollib {
	namespace Physics_function {

		//Õ“Ë‰ğŒˆ
		void resolve_contact(std::list<ALP_Collider*>& colliders, std::vector<Contacts::Contact_pair*>& pairs, std::list<Physics_function::ALP_Joint*> joints);

	}
}