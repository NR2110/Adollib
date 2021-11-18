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
			float ray_offset = 0;          // Raymin�̍ŏ��̒l
			float raymin = 0;              // Ray�̏Փ˂����ŏ��l
			float raymax = 0;              // Ray�̏Փ˂����ő�l
			Vector3 normal;                // Ray�̏Փ˂����ŋ߂̏ꏊ�̖@��
			Collider* coll = nullptr;      // Ray�̏Փ˂����ŋ߂�Collider
		};
	public:
		bool ray_cast(Raycast_struct& str);

		bool sphere_cast(const float& radius, Vector3& contact_point, Raycast_struct& str);
		bool sphere_cast(const float& radius, Raycast_struct& str) { Vector3 cp;
		return sphere_cast(radius, cp, str);
		};

	};

}