#pragma once

#include "joint_base.h"
#include "collider.h"
#include "../Object/gameobject.h"

#include "../Imgui/debug.h"

namespace Adollib {
	class Collider;

	namespace Physics_function {
		class ALP_Joint;

		// �q���W�W���C���g
		class HingeJoint : public Joint_base {
		public:
			HingeJoint(Collider* l_colliderA_comp, Collider* l_colliderB_comp, ALP_Joint* l_ALPjoint)
				: Joint_base(l_colliderA_comp, l_colliderB_comp, l_ALPjoint) {}

			Anchor anchor_s; //Hinge �n�_
			Anchor anchor_g; //Hinge �I�_

			float hinge_pow = 0; //�W���C���g�̋��� �傫������Ǝ��������Ȃ��Ȃ�?

			Vector2 limit = Vector2(0, 360);

		public:
			void adapt_anchor() override {
				anchor_count = 2;

				anchors[0] = anchor_s;
				anchors[1] = anchor_g;
			}

			Matrix33 tensor_effect(const Collider* coll) const override {
				Vector3 axis;
				if (coll == collider_comp[0])axis = anchor_s.posA - anchor_g.posA;
				else axis = anchor_s.posB - anchor_g.posB;

				Matrix33 mat = matrix33_zero();

				axis.x = fabsf(axis.x);
				axis.y = fabsf(axis.y);
				axis.z = fabsf(axis.z);

				mat._11 = axis.y * axis.y + axis.z * axis.z;
				mat._22 = axis.z * axis.z + axis.x * axis.x;
				mat._33 = axis.x * axis.x + axis.y * axis.y;

				mat *= hinge_pow;

				return mat;
			}

			bool limit_effect(Vector3& contactP0, Vector3& contactP1, float& penetrate) const override {

				constexpr float power = 2; //�� �Ȃ����Ȃ��ƒ���t��

				// transform
				const Transfome* transforms[2] = {
					collider_comp[0]->gameobject->transform.get(),
					collider_comp[1]->gameobject->transform.get()
				};

				//Hinge�̌���
				const Vector3 hinge_vec[2] = {
					(anchor_s.posA - anchor_g.posA).unit_vect(),
					(anchor_s.posB - anchor_g.posB).unit_vect()
				};
				//Hinge��world_vec
				const Vector3 hinge_vec_world[2] = {
					vector3_quatrotate(hinge_vec[0],transforms[0]->orientation),
					vector3_quatrotate(hinge_vec[1],transforms[1]->orientation),
				};
				//Hinge��90�x��]������local_vec
				const Vector3 hinge_vec_rot90[2] = {
					vector3_quatrotate(Vector3(1,0,0),quaternion_from_to_rotate(Vector3(0,1,0),hinge_vec[0])),
					vector3_quatrotate(Vector3(1,0,0),quaternion_from_to_rotate(Vector3(0,1,0),hinge_vec[1]))
				};
				const Vector3 hinge_vec_rot90_world[2] = {
					vector3_quatrotate(hinge_vec_rot90[0],transforms[0]->orientation).unit_vect(),
					vector3_quatrotate(hinge_vec_rot90[1],transforms[1]->orientation).unit_vect(),
				};

				//�p�x�𓾂�
				float radian = vector3_radian(hinge_vec_rot90_world[0], hinge_vec_rot90_world[1]);
				if (vector3_dot(hinge_vec_world[0], vector3_cross(hinge_vec_rot90_world[0], hinge_vec_rot90_world[1])) < 0) {
					radian = DirectX::XM_PI + DirectX::XM_PI - radian; //0~180~0 �� 0~360�Ɏ���
				};

				const Vector2 limit_rad = Vector2(ToRadian(limit.x), ToRadian(limit.y)); //limit��radian�Ɏ�����

				Debug::set("hinge_angle", ToAngle(radian));

				// ����limit�̉e�����󂯂�ʒu�ɓ��Ȃ����false��return
				if (limit_rad.x <= limit_rad.y) {
					if (limit_rad.x <= radian && radian <= limit_rad.y)return false;
				}
				else if (limit_rad.y <= limit_rad.x) {
					if (!(limit_rad.y <= radian && radian <= limit_rad.x))return false;
				}

				//radian�����cos�ň�ԋ߂�limit�����߂�
				const Vector2 limit_rad_off = Vector2(limit_rad.x - radian, limit_rad.y - radian);

				//contactP1�̊��
				//contactP0�������Ă���
				if (cosf(limit_rad_off.x) < cosf(limit_rad_off.y)) {
					Vector3 contactP0_world = vector3_quatrotate(hinge_vec_rot90_world[1], quaternion_axis_radian(hinge_vec_world[1], limit_rad_off.y)) + transforms[1]->position;

					//limit_x�̂ق����߂�
					contactP0 = vector3_quatrotate(contactP0_world - transforms[0]->position, transforms[0]->orientation.inverse());
					contactP1 = hinge_vec_rot90[1];

					penetrate = 2 * cosf(DirectX::XM_PIDIV2 - fabsf(limit_rad_off.y) * 0.5f) * power;
				}
				else {
					Vector3 contactP0_world = vector3_quatrotate(hinge_vec_rot90_world[1], quaternion_axis_radian(hinge_vec_world[1], limit_rad_off.x)) + transforms[1]->position;

					//limit_y�̂ق����߂�
					contactP0 = vector3_quatrotate(contactP0_world - transforms[0]->position, transforms[0]->orientation.inverse());
					contactP1 = hinge_vec_rot90[1] ;

					penetrate = 2 * cosf(DirectX::XM_PIDIV2 - fabsf(limit_rad_off.x) * 0.5f) * power;
				}

				return true;
			}



		};

	}

};
