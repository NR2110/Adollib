#pragma once
#include <memory>
#include "contact.h"
#include "ALP_physics.h"
namespace Adollib {
	namespace physics_function {
		//とても大雑把な当たり判定(Boardphase)
		void Broadphase(std::list<ALP_Physics>& ALP_physicses, std::vector<Contacts::Collider_2>& out_pair);

	}
}