#pragma once
#include "contact.h"

namespace Adollib {
	namespace physics_function {
		//‘åŽG”c‚È“–‚½‚è”»’è(Boardphase)
		void Broadphase(const std::vector<Collider*>& coll, std::vector<Contacts::Contact_pair>& pairs);

	}
}