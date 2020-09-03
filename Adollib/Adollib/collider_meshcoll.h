#pragma once
#include "collider__base.h"
#include "meshcoll_resorce_manager.h"
namespace Adollib {
	//Mesh colliderクラス
	class Meshcoll : public Collider {
	public:
		vector3 half_size = vector3();
		vector3 offset = vector3();
		DOP_14 dopbase;

		std::vector<int> index;
		std::vector<vector3> vertex;

		//不動オブジェクトとして生成
		Meshcoll(const char* filename, float density, vector3 pos = vector3(0, 0, 0)) {
			//shapeの設定
			shape = Collider_shape::shape_mesh;

			Collider_ResourceManager::CreateMCFromFBX(filename, &index, &vertex);

			vector3 half_max = vector3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
			vector3 half_min = vector3(FLT_MAX, FLT_MAX, FLT_MAX);

			memset(dopbase.max, -FLT_MAX, DOP_size);
			memset(dopbase.min,  FLT_MAX, DOP_size);

			//初期状態のDOPの保存 回転などを考慮したのDOPはこのDOPを基準にする
			for (int i = 0; i < DOP_size; i++) {
				for (vector3& v : vertex) {

					float dis = fabsf(vector3_dot(DOP_14_axis[i], v));
					if (dopbase.max[i] < dis) dop14.max[i] = +dis * 1.00000001f;//確実にするためちょっと大きめにとる
					if (dopbase.min[i] > dis) dop14.min[i] = +dis * 1.00000001f;
				}
			}

			half_size = (half_max + half_min) * 0.5f;
			offset = half_max - half_min;

			//密度の保存
			this->density = density;

			//座標
			local_position = pos;

			//質量の計算
			inertial_mass = (half_size.x * half_size.y * half_size.z) * 8.0f * density;

			//慣性モーメントの計算
			inertial_tensor = matrix_identity();
			inertial_tensor._11 = 0.3333333f * inertial_mass * ((half_size.y * half_size.y) + (half_size.z * half_size.z));
			inertial_tensor._22 = 0.3333333f * inertial_mass * ((half_size.z * half_size.z) + (half_size.x * half_size.x));
			inertial_tensor._33 = 0.3333333f * inertial_mass * ((half_size.x * half_size.x) + (half_size.y * half_size.y));
		}

		//サイズの所得関数のオーバーライド
		quaternion get_dimension() const {
			return half_size;
		}
		//world変換関数のオーバーライド
		void update_world_trans();

		//sizeや密度が変更されると質量や完成モーメントの変更が必要になるからそのために用意
		void update_inertial(const vector3& half_size, float density = 1) {
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