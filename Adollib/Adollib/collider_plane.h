#pragma once
#include "collider__base.h"

namespace Adollib {
	//平面用クラス
	class Plane : public Collider {
	public:
		//不動オブジェクトとして生成
		Plane(Vector3 n = Vector3(0, 1, 0), float d = 0) {
			//shapeの設定
			shape = Collider_shape::shape_plane;

			//質量の計算
			inertial_mass = FLT_MAX;

			//慣性モーメントの計算
			inertial_tensor = matrix_identity();
			inertial_tensor._11 = FLT_MAX;
			inertial_tensor._22 = FLT_MAX;
			inertial_tensor._33 = FLT_MAX;

			//動かない
			move = false;

			n = n.unit_vect();
			local_position = n * d;

			Vector3 Y(0, 1, 0);
			float angle = acosf(vector3_dot(Y, n));
			Vector3 axis;
			axis = vector3_cross(Y, n);
			axis = axis.unit_vect();
			local_orientation = quaternion_angle_axis(angle, axis);
			local_orientation = local_orientation.unit_vect();
		}

		//サイズの所得関数のオーバーライド
		Quaternion get_dimension() const {
			return Quaternion(1, 0, 1);
		}
		//world変換関数のオーバーライド
		void update_world_trans();
		//sizeや密度が変更されると質量や完成モーメントの変更が必要になるからそのために用意(planeは処理なし)
		void update_inertial(const Vector3& size, float density = 1) {
			//float r = size.x;
			////質量の計算
			//inertial_mass = 4.0f / 3.0f * r * r * r * DirectX::XM_PI * density;

			////慣性モーメントの計算
			//inertial_tensor = matrix_identity();
			//inertial_tensor._11 = 0.4f * inertial_mass * r * r;
			//inertial_tensor._22 = 0.4f * inertial_mass * r * r;
			//inertial_tensor._33 = 0.4f * inertial_mass * r * r;
		}
		//dop6の更新
		void update_dop14();

	};

}