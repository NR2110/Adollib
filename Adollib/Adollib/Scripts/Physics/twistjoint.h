#pragma once

#include "joint_base.h"
#include "collider.h"
#include "../Object/gameobject.h"

#include "../Imgui/debug.h"

namespace Adollib {
	class Collider;

	namespace Physics_function {
		class ALP_Joint;
	}

	// ひねり拘束(ジョイント?)
	// collider0を基準に
	// collider1の回転量に制限を加える
	class TwistJoint : public Joint_base {
	public:
		TwistJoint(Collider* l_colliderA_comp, Collider* l_colliderB_comp, Physics_function::ALP_Joint* l_ALPjoint)
			: Joint_base(l_colliderA_comp, l_colliderB_comp, l_ALPjoint) {}

		Vector3 vec0 = Vector3(1, 0, 0); //collider_comp[0]の軸
		Vector3 vec1 = Vector3(1, 0, 0); //collider_comp[1]の軸

		float max_radian_pow = 10; //角度の差が大きすぎると跳ねてきもいので limit_effectで力を加える時に考慮する角度の最大値

		Vector2 limit = Vector2(0, 360);

	public:
		void adapt_anchor() override {
			anchor_count = 0;
		}

		Matrix33 tensor_effect() const override {
			return matrix33_zero();
		}

		bool limit_effect(Vector3& contactP0, Vector3& contactP1, float& penetrate) const override;



	};

};
