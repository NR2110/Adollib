#pragma once
#include "../Math/math.h"

namespace Adollib {

	class Collider;

	class Ray {
	public:
		Vector3 position;
		Vector3 direction;

		struct Raycast_struct {
			u_int collider_tag = UINT_MAX; // �Փ˂���tag
			float ray_min = 0; // Raymin�̍ŏ��̒l
			float raymin = 0; // Ray�̏Փ˂����ŏ��l
			float raymax = 0; // Ray�̏Փ˂����ő�l
			Vector3 normal; // Ray�̏Փ˂����ŋ߂̏ꏊ�̖@��
			Collider* coll = nullptr; // Ray�̏Փ˂����ŋ߂�Collider
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