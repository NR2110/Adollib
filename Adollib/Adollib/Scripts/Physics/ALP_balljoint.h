#pragma once

#include "ALP_joint_base.h"

namespace Adollib {

	namespace Physics_function {
		class ALP_Collider;

		// ボールジョイント
		struct ALP_BallJoint : public ALP_Joint_base {

			// 初期化
			void reset()
			{
				bias = 0.1f;
				constraint.accuminpulse = 0.0f;
			}

		void vvv() override;

		};





	}

}
