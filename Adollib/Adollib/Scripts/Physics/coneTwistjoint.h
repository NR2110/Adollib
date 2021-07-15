#pragma once

#include "joint_base.h"

namespace Adollib {
	class Collider;

	namespace Physics_function {
		class ALP_Joint;

		// Conetwist 肩などに使用
		class ConetwistJoint : public Joint_base {
		public:
			ConetwistJoint(Collider* l_colliderA_comp, Collider* l_colliderB_comp, ALP_Joint* l_ALPjoint)
				: Joint_base(l_colliderA_comp, l_colliderB_comp, l_ALPjoint) {}

			Anchor anchor; //接続点
			Vector3 limit_axis[2]; //軸 axisとaxisの角度でlimitをとる

			float limit = 0;

		public:
			void adapt_anchor() override {
				anchor_count = 1;

				anchors[0] = anchor;
			}

			Matrix33 tensor_effect(const Collider* coll) const override {
				return matrix33_zero();
			}

			bool limit_effect(Vector3& contactP0, Vector3& contactP1, float& penetrate) const override {

				//limit_axisをworldに持ってくる
				const Vector3 limit_axis_world[2] = {
					vector3_quatrotate(limit_axis[0],collider_comp[0]->transform->orientation),
					vector3_quatrotate(limit_axis[1],collider_comp[1]->transform->orientation)
				};

				//角度を得る
				float radian = vector3_radian(limit_axis_world[0], limit_axis_world[1]); // 0 - π

				const float limit_rad = ToRadian(limit); //limitえおradianに治した

				Debug::set("angle", ToAngle(radian));

				// もしlimitの影響を受ける位置に入なければfalseをreturn
				if (radian <= limit_rad) return false;

				//collider_comp[0]のaxisを基準にして考える

				//limit_axis_world[0]を基準にしたlimitのvector_worldcoord
				Vector3 limit_world = vector3_quatrotate(limit_axis_world[0], quaternion_radian_axis(limit_rad, vector3_cross(limit_axis_world[0], limit_axis_world[1])));
				{
					contactP0 = vector3_quatrotate(limit_world, collider_comp[0]->transform->orientation.inverse());
					contactP1 = limit_axis[1];

					penetrate = (limit_world - limit_axis_world[1]).norm_sqr();
				}


				return true;
			}



		};

	}

};
