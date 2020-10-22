#pragma once
#include "collider__base.h"

namespace Adollib {
	//Boxクラス
	class Box : public Collider {
	public:
		Vector3 half_size = Vector3(1,1,1); //localサイズ

		//	std::vector<std::string> no_hit_tag = std::vector<std::string>()
		Box(Vector3 half_size = Vector3(1), float density = 1, Vector3 pos = Vector3(0, 0, 0)) : half_size(half_size) {
			//shapeの設定
			shape = Collider_shape::shape_box;

			//密度の保存
			this->density = density;

			//座標
			local_position = pos;

			local_scale = Vector3(1);

			local_orientation = Quaternion(1, 0, 0, 0);

			//質量の計算
			inertial_mass = (half_size.x * half_size.y * half_size.z) * 8.0f * density;

			//慣性モーメントの計算
			inertial_tensor = matrix_identity();
			inertial_tensor._11 = 0.3333333f * inertial_mass * ((half_size.y * half_size.y) + (half_size.z * half_size.z));
			inertial_tensor._22 = 0.3333333f * inertial_mass * ((half_size.z * half_size.z) + (half_size.x * half_size.x));
			inertial_tensor._33 = 0.3333333f * inertial_mass * ((half_size.x * half_size.x) + (half_size.y * half_size.y));
		}

		//サイズの所得関数のオーバーライド
		Quaternion get_dimension() const {
			return half_size;
		}
		//world変換関数のオーバーライド
		void update_world_trans();

		//sizeや密度が変更されると質量や完成モーメントの変更が必要になるからそのために用意
		void update_inertial(const Vector3& half_size, float density = 1) {
			this->density = density;

			//質量の計算
			inertial_mass = (half_size.x * half_size.y * half_size.z) * 8.0f * density;

			//慣性モーメントの計算
			inertial_tensor = matrix_identity();
			inertial_tensor._11 = 0.3333333f * inertial_mass * ((half_size.y * half_size.y) + (half_size.z * half_size.z));
			inertial_tensor._22 = 0.3333333f * inertial_mass * ((half_size.z * half_size.z) + (half_size.x * half_size.x));
			inertial_tensor._33 = 0.3333333f * inertial_mass * ((half_size.x * half_size.x) + (half_size.y * half_size.y));
		}
		//dop6の更新
		void update_dop14();

	};
}