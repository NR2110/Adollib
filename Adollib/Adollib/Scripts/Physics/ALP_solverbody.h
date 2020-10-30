#pragma once
#include "../Math/math.h"

namespace Adollib {
	namespace physics_function {
		enum class Collider_shape {
			shape_box,
			shape_sphere,
			shape_plane,
			shape_mesh,

			shape_size
		};

		struct Solverbody {
			Vector3 delta_LinearVelocity; // •Ài‘¬“x·•ª
			Vector3 delta_AngulaVelocity; // ‰ñ“]‘¬“x·•ª
			Quaternion orientation; // p¨
			Matrix inv_inertia; // Šµ«ƒeƒ“ƒ\ƒ‹‚Ì‹ts—ñ
			float  inv_mass; // ¿—Ê‚Ì‹t”
			int num;
		};

	}
}