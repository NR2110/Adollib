#pragma once
#include "../Math/math.h"

namespace Adollib {

	class Collider;

	class Ray {
	public:
		Vector3 position;
		Vector3 direction;

		struct Raycast_struct {
			u_int collider_tag = UINT_MAX; // 衝突するtag
			float ray_offset = 0;          // Rayminの最小の値
			float raymin = 0;              // Rayの衝突した最小値
			float raymax = 0;              // Rayの衝突した最大値
			Vector3 normal;                // Rayの衝突した最近の場所の法線
			Collider* coll = nullptr;      // Rayの衝突した最近のCollider
		};
	public:
		bool ray_cast(Raycast_struct& str);

		bool sphere_cast(const float& radius, Vector3& contact_point, Raycast_struct& str);
		bool sphere_cast(const float& radius, Raycast_struct& str) { Vector3 cp;
		return sphere_cast(radius, cp, str);
		};

	};

}