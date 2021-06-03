#pragma once

namespace Adollib {
	namespace Physics_function {


		enum class ALPCollider_shape_type {
			None,
			BOX,
			Sphere,
			Plane,
			Capsule,
			Mesh,
			Cylinder
		};

		enum class Tensor_type {
			None,
			Sphere,
			Box
		};
	}
}