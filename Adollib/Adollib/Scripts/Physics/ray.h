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
			float ray_min = 0; // Rayminの最小の値
			float raymin = 0; // Rayの衝突した最小値
			float raymax = 0; // Rayの衝突した最大値
			Vector3 normal; // Rayの衝突した最近の場所の法線
			Collider* coll = nullptr; // Rayの衝突した最近のCollider
		};
	public:
		bool ray_cast(u_int tag,
			const float ray_min,
			float& tmin, float& tmax, Vector3& normal, Collider*& coll);

		bool ray_cast(Raycast_struct& str) {
			Vector3 normal;
			return ray_cast(str.collider_tag, str.ray_min, str.raymin, str.raymax, str.normal, str.coll);
		}



	};

}