#pragma once
#include "contact.h"

namespace Adollib {
	namespace physics_function {
		//�ƂĂ���G�c�ȓ����蔻��(Boardphase)
		void Broadphase(const std::vector<Collider*>& coll, std::vector<Contacts::Contact_pair>& pairs);

	}
}