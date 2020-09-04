#pragma once
#include "contact.h"
#include <memory>
namespace Adollib {
	namespace physics_function {
		//�ƂĂ���G�c�ȓ����蔻��(Boardphase)
		void Broadphase(std::list<std::shared_ptr<Adollib::Collider>>& coll, std::vector<Contacts::Collider_2>& out_pair, std::vector<Contacts::Contact_pair>& pairs);

	}
}