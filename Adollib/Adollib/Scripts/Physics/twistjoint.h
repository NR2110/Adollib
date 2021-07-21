#pragma once

#include "joint_base.h"
#include "collider.h"
#include "../Object/gameobject.h"

#include "../Imgui/debug.h"

namespace Adollib {
	class Collider;

	namespace Physics_function {
		class ALP_Joint;

		// ひねり拘束(ジョイント?)
		// collider0を基準に
		// collider1の回転量に制限を加える
		class TwistJoint : public Joint_base {
		public:
			TwistJoint(Collider* l_colliderA_comp, Collider* l_colliderB_comp, ALP_Joint* l_ALPjoint)
				: Joint_base(l_colliderA_comp, l_colliderB_comp, l_ALPjoint) {}

			// collider_component1の軸
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

				constexpr float power = 2; //謎 なぜかないと張り付く

				const Transfome* transforms[2]{
					collider_comp[0]->gameobject->transform.get(),
					collider_comp[1]->gameobject->transform.get()
				};

				// collider_comp[1]からcollider_comp[0]へのオフセット (collider_comp[1]のlocal情報をcollider_comp[0]の座標系にする)
				Quaternion off_comp1to0 = transforms[1]->orientation * transforms[0]->orientation.inverse();

				// collider_comp[0]の座標系である
				Vector3 vec1_comp0coord= vector3_quatrotate(vec1, off_comp1to0).unit_vect();

				// 回転前、回転後のお互いに垂直なベクトル -> z回転以外の影響を受けない
				Vector3 axis = vector3_cross(vec1, vec1_comp0coord);
				if (fabsf(fabsf(vector3_dot(vec1, vec1_comp0coord)) - 1) < FLT_EPSILON) {
					//2ベクトルが平行なら適当な垂直な軸でよい
					axis = vector3_cross(vec1, vec1_comp0coord + Vector3(1, 0, 0));
				}
				axis = axis.unit_vect();


				// axisの回転後のベクトル
				Vector3 axis_comp0coord =  vector3_quatrotate(axis, off_comp1to0);

				// z回転の角度を得る
				float radian = vector3_radian(axis_comp0coord, axis);
				if (vector3_dot(vector3_cross(axis_comp0coord, axis), vec1_comp0coord) < 0) {
					radian = DirectX::XM_PI + DirectX::XM_PI - radian; //0~180~0 を 0~360に治す
					axis *= -1;
					axis_comp0coord *= -1;
				};

				const Vector2 limit_rad = Vector2(ToRadian(limit.x), ToRadian(limit.y)); //limitをradianに治した

				Debug::set("twist_angle", ToAngle(radian));

				// もしlimitの影響を受ける位置に入なければfalseをreturn
				if (limit_rad.x <= limit_rad.y) {
					if (limit_rad.x <= radian && radian <= limit_rad.y)return false;
				}
				else if (limit_rad.y <= limit_rad.x) {
					if (!(limit_rad.y <= radian && radian <= limit_rad.x))return false;
				}

				//radianを基準にcosで一番近いlimitを求める
				const Vector2 limit_rad_off = Vector2(limit_rad.x - radian, limit_rad.y - radian);

				//contactP1の基準に
				//contactP0を持ってくる
				if (cosf(limit_rad_off.x) < cosf(limit_rad_off.y)) {
					Vector3 contactP0_world = vector3_quatrotate(axis_comp0coord, quaternion_axis_radian(vec1_comp0coord, -limit_rad_off.y));

					//limit_xのほうが近い
					contactP0 = vector3_quatrotate(transforms[1]->position - transforms[0]->position, transforms[0]->orientation.inverse()) + contactP0_world;
					contactP1 = axis;

					penetrate = 2 * cosf(DirectX::XM_PIDIV2 - fabsf(limit_rad_off.y) * 0.5f);
				}
				else {
					Vector3 contactP0_world = vector3_quatrotate(axis_comp0coord, quaternion_axis_radian(vec1_comp0coord, -limit_rad_off.x));

					//limit_yのほうが近い
					contactP0 = vector3_quatrotate(transforms[1]->position - transforms[0]->position, transforms[0]->orientation.inverse()) + contactP0_world;
					contactP1 = axis;

					penetrate = 2 * cosf(DirectX::XM_PIDIV2 - fabsf(limit_rad_off.x) * 0.5f);
				}

				return true;
			}



		};

	}

};
