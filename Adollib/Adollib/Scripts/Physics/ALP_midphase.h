#pragma once
#include "contact.h"
#include "collider_shape.h"

namespace Adollib {
	namespace Physics_function {
		//‘åŽG”c‚È“–‚½‚è”»’è
		void Midphase(std::vector<Contacts::Collider_2>& out_pair, std::vector<Contacts::Contact_pair>& pairs);

	}
}