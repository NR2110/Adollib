#pragma once

#include "joint_base.h"
#include "collider.h"
#include "../Object/gameobject.h"

#include "../Imgui/debug.h"

namespace Adollib {
	class Collider;

	namespace Physics_function {
		class ALP_Joint;

		// �Ђ˂�S��(�W���C���g?)
		// collider0�����
		// collider1�̉�]�ʂɐ�����������
		class TwistJoint : public Joint_base {
		public:
			TwistJoint(Collider* l_colliderA_comp, Collider* l_colliderB_comp, ALP_Joint* l_ALPjoint)
				: Joint_base(l_colliderA_comp, l_colliderB_comp, l_ALPjoint) {}

			// collider_component1�̎�
			Vector3 vec1 = Vector3(1, 0, 0);

			Vector2 limit = Vector2(0, 360);

		public:
			void adapt_anchor() override {
				anchor_count = 0;
			}

			Matrix33 tensor_effect(const Collider* coll) const override {
				return matrix33_zero();
			}

			bool limit_effect(Vector3& contactP0, Vector3& contactP1, float& penetrate) const override {

				constexpr float power = 2; //�� �Ȃ����Ȃ��ƒ���t��

				const Transfome* transforms[2]{
					collider_comp[0]->gameobject->transform.get(),
					collider_comp[1]->gameobject->transform.get()
				};

				// collider_comp[1]����collider_comp[0]�ւ̃I�t�Z�b�g (collider_comp[0]��e collider_comp[1]���q �Ƃ�����])
				Quaternion off_comp1to0 = transforms[0]->orientation * transforms[1]->orientation.inverse();

				// world�Ƃ������O���� collider_comp[0]�̍��W�n�ł���
				Vector3 vec1_rotated = vector3_quatrotate(vec1, transforms[1]->orientation);

				// ��]�O�A��]��̂��݂��ɐ����ȃx�N�g�� -> z��]�ȊO�̉e�����󂯂Ȃ�
				Vector3 axis = vector3_cross(vec1, vec1_rotated);
				if (fabsf(fabsf(vector3_dot(vec1, vec1_rotated)) - 1) < FLT_EPSILON) {
					//2�x�N�g�������s�Ȃ�K���Ȑ����Ȏ��ł悢
					axis = vector3_cross(vec1, vec1_rotated + Vector3(1, 0, 0));
				}
				axis = axis.unit_vect();


				// axis�̉�]��̃x�N�g��
				Vector3 axis_rotated =  vector3_quatrotate(axis, transforms[1]->orientation);

				Debug::set("axis", axis);
				Debug::set("axis_rotated", axis_rotated);

				// z��]�̊p�x�𓾂�
				float radian = vector3_radian(axis_rotated, axis);
				if (vector3_dot(vector3_cross(axis_rotated, axis), vec1_rotated) < 0) {
					radian = DirectX::XM_PI + DirectX::XM_PI - radian; //0~180~0 �� 0~360�Ɏ���
					axis *= -1;
					axis_rotated *= -1;
				};

				const Vector2 limit_rad = Vector2(ToRadian(limit.x), ToRadian(limit.y)); //limit��radian�Ɏ�����

				Debug::set("angle", ToAngle(radian));
				Debug::set("quate_angle", ToAngle(acosf(transforms[1]->orientation.z) * 2));

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
					Vector3 contactP0_world = vector3_quatrotate(axis_rotated, quaternion_radian_axis(-limit_rad_off.y, vec1_rotated)) + transforms[1]->position;

					//limit_x�̂ق����߂�
					contactP0 = vector3_quatrotate(contactP0_world - transforms[0]->position, transforms[0]->orientation.inverse());
					contactP1 = axis;

					penetrate = 2 * cosf(DirectX::XM_PIDIV2 - fabsf(limit_rad_off.y) * 0.5f);
				}
				else {
					Vector3 contactP0_world = vector3_quatrotate(axis_rotated, quaternion_radian_axis(-limit_rad_off.x, vec1_rotated)) + transforms[1]->position;

					//limit_y�̂ق����߂�
					contactP0 = vector3_quatrotate(contactP0_world - transforms[0]->position, transforms[0]->orientation.inverse());
					contactP1 = axis;

					penetrate = 2 * cosf(DirectX::XM_PIDIV2 - fabsf(limit_rad_off.x) * 0.5f);
				}

				return true;
			}



		};

	}

};