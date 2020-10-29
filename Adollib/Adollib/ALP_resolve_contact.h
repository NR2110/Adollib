#pragma once
#include "compute_shader.h"
#include "contact.h"
#include <memory>
namespace Adollib {
	namespace physics_function {
		struct Solverbody {
			Vector3 delta_LinearVelocity; // •Ài‘¬“x·•ª
			Vector3 delta_AngulaVelocity; // ‰ñ“]‘¬“x·•ª
			Quaternion orientation; // p¨
			matrix inv_inertia; // Šµ«ƒeƒ“ƒ\ƒ‹‚Ì‹ts—ñ
			float  inv_mass; // ¿—Ê‚Ì‹t”
			int num;
		};

		//Õ“Ë‰ğŒˆ
		void resolve_contact(std::list<Adollib::Collider*>, std::vector<Contacts::Contact_pair>& pairs);

	}
}