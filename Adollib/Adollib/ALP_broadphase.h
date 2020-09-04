#pragma once
#include "contact.h"
#include <memory>
namespace Adollib {
	namespace physics_function {
		//とても大雑把な当たり判定(Boardphase)
		void Broadphase(std::list<std::shared_ptr<Adollib::Collider>>& coll, std::vector<Contacts::Collider_2>& out_pair, std::vector<Contacts::Contact_pair>& pairs);

	}
}