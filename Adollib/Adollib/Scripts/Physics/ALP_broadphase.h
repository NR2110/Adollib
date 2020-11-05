#pragma once
#include <memory>
#include "contact.h"
#include "ALP_physics.h"
namespace Adollib {
	namespace physics_function {
		//‚Æ‚Ä‚à‘åŽG”c‚È“–‚½‚è”»’è(Boardphase)
		void Broadphase(std::list<ALP_Physics>& ALP_physicses, std::vector<Contacts::Collider_2>& out_pair);

	}
}