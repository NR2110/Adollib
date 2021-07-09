#pragma once

#include "../Math/math.h"
#include "collider.h"

namespace Adollib {

	namespace Physics_function {
		class ALP_Collider;
		class Joint_base;
		class BallJoint;
		class HingeJoint;
	}

	class Joint {
	public:
		//Jointの削除
		static void delete_joint(Physics_function::Joint_base*&);

		//BallJointの生成
		static Physics_function::BallJoint* add_balljoint(
			Collider* colliderA,Collider* colliderB,
			Vector3 anchorA, Vector3 anchorB,
			float bias = 0.1f
		);

		//HingeJointの生成
		static Physics_function::HingeJoint* add_Hingejoint(
			Collider* colliderA, Collider* colliderB,
			Vector3 anchorA_s, Vector3 anchorA_g,
			Vector3 anchorB_s, Vector3 anchorB_g,
			float hingepow = 0.1f,
			float bias = 0.1f
		);
	};



}