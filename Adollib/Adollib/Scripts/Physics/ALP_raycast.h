#pragma once
#include "../Math/math.h"
#include "ALP_collider.h"

namespace Adollib {
	namespace Physics_function {

		//::::::
		// ray_pos : ray�̎n�_
		// ray_dir : ray�̌���
		// ray_min : ray�̎n�_�̋���
		// ----out----
		// coll : �Փ˂���collider�ւ̃|�C���^
		// tmin ���� tmax
		// normal : �Փ˂����ʂ�normal
		bool ray_cast(const Vector3& Ray_pos, const Vector3& Ray_dir,
			const float ray_min,
			const ALP_Collider* coll,
			float& tmin, float& tmax,
			Vector3& normal
			);

		//::::::
		// ray_pos : ray�̎n�_
		// ray_dir : ray�̌���
		// radius  : ���̔��a
		// ray_min : ray�̎n�_�̋���
		// ----out----
		// contact_point : �Փ˂����_
		// coll : �Փ˂���collider�ւ̃|�C���^
		// tmin ���� tmax
		// normal : �Փ˂����ʂ�normal
		bool sphere_cast(const Vector3& Ray_pos, const Vector3& Ray_dir, const float& radius, Vector3& contact_point,
			const float ray_min,
			const ALP_Collider* coll,
			float& tmin, float& tmax,
			Vector3& normal
		);

	}
}