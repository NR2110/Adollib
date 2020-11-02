#pragma once
#include <memory>
#include "ALP_collider.h"
#include "ALP_physics.h"
#include "ALP_shape.h"

namespace Adollib {
	namespace physics_function {
		class ALP__CollPhysicsShape_data {
		public:
			 ALP_Collider ALP_coll;
			 ALP_Physics ALP_physics;
			 ALP_Shape ALP_shape;

		public:
			Collider* coll;


		};
	}
}
