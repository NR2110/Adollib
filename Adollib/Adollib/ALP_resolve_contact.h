#pragma once
#include "compute_shader.h"
#include "contact.h"

namespace Adollib {
	namespace physics_function {
		struct Solverbody {
			vector3 delta_LinearVelocity; // •Ài‘¬“x·•ª
			vector3 delta_AngularVelocity; // ‰ñ“]‘¬“x·•ª
			quaternion orientation; // p¨
			matrix inv_inertia; // Šµ«ƒeƒ“ƒ\ƒ‹‚Ì‹ts—ñ
			float  inv_mass; // ¿—Ê‚Ì‹t”
			int num;
		};

		//Õ“Ë‰ğŒˆ
		void resolve_contact(std::vector<Collider*> colliders, std::vector<Contacts::Contact_pair>& pairs);

	}
}