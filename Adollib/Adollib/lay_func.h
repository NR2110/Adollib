#pragma once
#include "vector.h"
#include "collider_box.h"

namespace Adollib {

	namespace Lay_func {
		class Lay {
		public:
			vector3 pos;
			vector3 dir;
		};

		bool intersectRayCollider(const Lay& lay, Collider* coll, float& min);
		bool intersectRayBox(const Lay& lay,const Box& box, float& min);


	}
}