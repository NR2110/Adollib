#pragma once

#include "../Math/math.h"
#include "collider.h"

#include "balljoint.h"
#include "hingejoint.h"
#include "coneTwistjoint.h"

namespace Adollib {

	namespace Physics_function {
		class ALP_Collider;
		class Joint_base;
		class BallJoint;
		class HingeJoint;
	}

	class Joint {
	public:
		//JointÇÃçÌèú
		static void delete_joint(Physics_function::Joint_base*&);

		//BallJointÇÃê∂ê¨
		static Physics_function::BallJoint* add_balljoint(
			Collider* colliderA, Collider* colliderB,
			const Vector3& anchorA, const Vector3& anchorB,
			const float& bias = 0.1f
		);

		//HingeJointÇÃê∂ê¨
		static Physics_function::HingeJoint* add_Hingejoint(
			Collider* colliderA, Collider* colliderB,
			const Vector3& anchorA_s, const Vector3& anchorA_g,
			const Vector3& anchorB_s, const Vector3& anchorB_g,
			const float& hingepow = 0.1f,
			const float& bias = 0.1f
		);

		//ConeTwistJointÇÃê∂ê¨
		static Physics_function::ConetwistJoint* add_Conetwistjoint(
			Collider* colliderA, Collider* colliderB,
			const Vector3& anchorA, const Vector3& anchorB,
			const Vector3& axisA, const Vector3& axisB,
			const float& bias = 0.1f
		);
	};



}