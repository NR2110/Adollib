#pragma once
#include "contact.h"
#include <memory>
namespace Adollib {
	namespace physics_function {
		//‚Æ‚Ä‚à‘åŽG”c‚È“–‚½‚è”»’è(Boardphase)
		void Broadphase(std::list<std::shared_ptr<Adollib::Collider>>& coll, std::vector<Contacts::Collider_2>& out_pair, std::vector<Contacts::Contact_pair>& pairs);

	}
}