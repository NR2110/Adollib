#pragma once
#include "contact.h"

namespace Adollib {
	namespace physics_function {
		//とても大雑把な当たり判定(Boardphase)
		void Broadphase(const std::vector<Collider*>& coll, std::vector<Contacts::Contact_pair>& pairs);

	}
}