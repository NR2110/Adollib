#pragma once

#include "../Math/math.h"

#include "balljoint.h"
#include "hingejoint.h"
#include "conejoint.h"
#include "coneTwistjoint.h"
#include "twistjoint.h"

namespace Adollib {
	class Collider;

	class Joint {
	public:
		//Joint�̍폜
		static void delete_joint(Joint_base*&);

		//BallJoint�̐���
		static BallJoint* add_balljoint(
			Collider* colliderA, Collider* colliderB,
			const Physics_function::Vector3& anchorA, const Physics_function::Vector3& anchorB,
			const float& bias = 0.1f
		);

		//HingeJoint�̐���
		static HingeJoint* add_Hingejoint(
			Collider* colliderA, Collider* colliderB,
			const Physics_function::Vector3& anchorA_s, const Physics_function::Vector3& anchorA_g,
			const Physics_function::Vector3& anchorB_s, const Physics_function::Vector3& anchorB_g,
			const float& hingepow = 0.01f,
			const float& bias = 0.1f
		);

		//ConeJoint�̐���
		static ConeJoint* add_Conejoint(
			Collider* colliderA, Collider* colliderB,
			const Physics_function::Vector3& anchorA, const Physics_function::Vector3& anchorB,
			const Physics_function::Vector3& axisA, const Physics_function::Vector3& axisB,
			const float& bias = 0.1f
		);

		//ConeTwistJoint�̐���
		static ConetwistJoint* add_Conetwistjoint(
			Collider* colliderA, Collider* colliderB,
			const Physics_function::Vector3& anchorA, const Physics_function::Vector3& anchorB,
			const Physics_function::Vector3& axisA, const Physics_function::Vector3& axisB,
			const float& bias = 0.1f
		);

		static TwistJoint* add_Twistjoint(
			Collider* colliderA, Collider* colliderB,
			const Physics_function::Vector3& axisA,
			const Physics_function::Vector3& axisB,
			const float& bias = 0.1f
		);

	};



}
