#pragma once

#include "../Math/math.h"
#include "collider.h"

namespace Adollib {

	namespace Physics_function {
		class ALP_Collider;
		class Joint_base;
		class BallJoint;
	}

	class Joint {
	public:
		static void delete_joint(Physics_function::Joint_base*&);

		static Physics_function::BallJoint* add_balljoint(
			Collider* colliderA, Vector3 anchorA,
			Collider* colliderB, Vector3 anchorB,
			float bias = 1
		);
	};



}