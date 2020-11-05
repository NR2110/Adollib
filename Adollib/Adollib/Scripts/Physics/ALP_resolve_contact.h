#pragma once
#include "contact.h"
#include <memory>
namespace Adollib {
	namespace physics_function {

		//Õ“Ë‰ğŒˆ
		void resolve_contact(std::list<ALP_Collider*>, std::vector<Contacts::Contact_pair>& pairs);

	}
}