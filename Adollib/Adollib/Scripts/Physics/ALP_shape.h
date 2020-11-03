#pragma once

#include "../Math/math.h"
#include "ALP_collider.h"

namespace Adollib {
	namespace physics_function {
		enum class ALP_Collider_shape {
			shape_box,
			shape_sphere,
			shape_plane,
			shape_mesh,

			shape_size
		};

		struct ALP_Solverbody {
			Vector3 delta_LinearVelocity; // 並進速度差分
			Vector3 delta_AngulaVelocity; // 回転速度差分
			Quaternion orientation; // 姿勢
			Matrix inv_inertia; // 慣性テンソルの逆行列
			float  inv_mass; // 質量の逆数
			int num;
		};

		class ALP_Shape {
		public:
			ALP_Collider_shape shape; 		//形情報
			DOP::DOP_14	dop14;			//14DOP データ
			ALP_Solverbody* solve; //衝突解決用

			ALP_Collider* ALP_coll;
		public:
			//DOP_14データの更新 ALP_Shape::dop14,ALP_Colider::scale,GOが必要なためここに記述
			void update_dop14();

		private:
			void update_dop14_as_sphere();
			void update_dop14_as_box();
			void update_dop14_as_plane();
			void update_dop14_as_mesh();

		};
	}
}
