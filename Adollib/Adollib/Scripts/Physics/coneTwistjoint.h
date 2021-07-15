#pragma once

#include "joint_base.h"

namespace Adollib {
	class Collider;

	namespace Physics_function {
		class ALP_Joint;

		// Conetwist ���ȂǂɎg�p
		class ConetwistJoint : public Joint_base {
		public:
			ConetwistJoint(Collider* l_colliderA_comp, Collider* l_colliderB_comp, ALP_Joint* l_ALPjoint)
				: Joint_base(l_colliderA_comp, l_colliderB_comp, l_ALPjoint) {}

			Anchor anchor; //�ڑ��_
			Vector3 limit_axis[2]; //�� axis��axis�̊p�x��limit���Ƃ�

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

				//limit_axis��world�Ɏ����Ă���
				const Vector3 limit_axis_world[2] = {
					vector3_quatrotate(limit_axis[0],collider_comp[0]->transform->orientation),
					vector3_quatrotate(limit_axis[1],collider_comp[1]->transform->orientation)
				};

				//�p�x�𓾂�
				float radian = vector3_radian(limit_axis_world[0], limit_axis_world[1]); // 0 - ��

				const float limit_rad = ToRadian(limit); //limit����radian�Ɏ�����

				Debug::set("angle", ToAngle(radian));

				// ����limit�̉e�����󂯂�ʒu�ɓ��Ȃ����false��return
				if (radian <= limit_rad) return false;

				//collider_comp[0]��axis����ɂ��čl����

				//limit_axis_world[0]����ɂ���limit��vector_worldcoord
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
