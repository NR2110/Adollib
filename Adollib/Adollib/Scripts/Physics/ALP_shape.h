#pragma once
#include "../Math/math.h"

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
			DOP::DOP_14	DOP_14;			//14DOP データ
			Solverbody* solve; //衝突解決用

		public:
			//オブジェクトが動くたびに呼び出す　のが効率よいが適当に毎フレーム呼ぶ
			//DOP_14データの更新 ALP_Shape::dop14,ALP_Colider::scale,GOが必要なためここに記述
			virtual void update_dop14() = 0;

		};
	}
}
