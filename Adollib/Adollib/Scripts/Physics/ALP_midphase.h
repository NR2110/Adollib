#pragma once
#include "contact.h"
#include "collider_shape.h"

namespace Adollib {
	namespace Physics_function {
		//��G�c�ȓ����蔻��
		void Midphase(std::vector<Contacts::Collider_2>& out_pair, std::vector<Contacts::Contact_pair>& pairs);

	}
}