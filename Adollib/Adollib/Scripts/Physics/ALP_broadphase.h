#pragma once
#include <memory>
#include "contact.h"
#include "ALP_physics.h"
namespace Adollib {
	namespace Physics_function {
		//�ƂĂ���G�c�ȓ����蔻��(Boardphase)
		void Broadphase(std::list<ALP_Collider>& ALP_physicses, std::vector<Contacts::Collider_2>& out_pair);

	}
}