#pragma once

#include "joint_base.h"
#include "conejoint.h"

#include "collider.h"
#include "../Object/transform.h"

#include "../Imgui/debug.h"

namespace Adollib {
	class Collider;

	namespace Physics_function {
		class ALP_Joint;
	}

	// Conetwist 肩などに使用
	class ConetwistJoint : public ConeJoint {
	public:
		ConetwistJoint(Collider* l_colliderA_comp, Collider* l_colliderB_comp, Physics_function::ALP_Joint* l_ALPjoint)
			: ConeJoint(l_colliderA_comp, l_colliderB_comp, l_ALPjoint) {}

		Anchor anchor; //接続点
		Vector3 limit_axis[2]; //軸 axisとaxisの角度でlimitをとる

		float limit = 90;

	public:
		void adapt_anchor() override {
			anchor_count = 1;

			anchors[0] = anchor;
		}

		Matrix33 tensor_effect(const Collider* coll) const override {
			return matrix33_zero();
		}

		bool limit_effect(Vector3& contactP0, Vector3& contactP1, float& penetrate) const override;

	};

};
