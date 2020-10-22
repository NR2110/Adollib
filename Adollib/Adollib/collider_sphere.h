#pragma once
#include "collider__base.h"

namespace Adollib {

	//球体用クラス
	class Sphere : public Collider {
	public:
		float r = 1; //半径

		Sphere(float r = 1, float density = 1, Vector3 pos = Vector3(0, 0, 0)) : r(r) {
			//shapeの設定
			shape = Collider_shape::shape_sphere;

			//密度の保存
			this->density = density;

			//座標
			local_position = pos;

			local_orientation = quaternion_identity();

			local_scale = Vector3(1);

			//質量の計算
			inertial_mass = 4.0f / 3.0f * r * r * r * DirectX::XM_PI * density;

			//慣性モーメントの計算
			inertial_tensor = matrix_identity();
			inertial_tensor._11 = 0.4f * inertial_mass * r * r;
			inertial_tensor._22 = 0.4f * inertial_mass * r * r;
			inertial_tensor._33 = 0.4f * inertial_mass * r * r;


		}

		//サイズの所得関数のオーバーライド
		Quaternion get_dimension() const {
			return Quaternion(r, r, r);
		}
		//world変換関数のオーバーライド
		void update_world_trans();
		//sizeや密度が変更されると質量や完成モーメントの変更が必要になるからそのために用意(球の半径 = size.x)
		void update_inertial(const Vector3& size, float density = 1) {
			this->density = density;

			float r = size.x;
			//質量の計算
			inertial_mass = 4.0f / 3.0f * r * r * r * DirectX::XM_PI * density;

			//慣性モーメントの計算
			inertial_tensor = matrix_identity();
			inertial_tensor._11 = 0.4f * inertial_mass * r * r;
			inertial_tensor._22 = 0.4f * inertial_mass * r * r;
			inertial_tensor._33 = 0.4f * inertial_mass * r * r;
		}
		//dop6の更新
		void update_dop14();

	};

}