#pragma once
#include "contact.h"

namespace Adollib {
	namespace physics_function {

		//��G�c�ȓ����蔻��(Boardphase)
		void Boardphase(const std::vector<Collider*>& coll, std::vector<Contacts::Contact_pair>& pairs);

	}
}