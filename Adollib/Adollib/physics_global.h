#pragma once

//:::::::::::::::::::
// 衝突判定で用いるconst変数の設定
//:::::::::::::::::::

namespace Adollib {

	struct physics_g {
		static constexpr int  Contact_max_per_pair = 4; //一つのペアで衝突の計算をする最大の数 基本的には4
		static constexpr float Allowable_error = 0.02f;  //これ以上近ければ同一衝突点とする
		static constexpr float Contact_threrhold_normal = 0.004f; // 衝突点の閾値（法線方向)
		static constexpr float Contact_threrhold_tangent = 0.02f;// 衝突点の閾値（平面上）

		static float bias; //貫通解決時に使用するばねの強さ
		static float slop; //貫通の許容誤差

		static float timeStep; //delta timeもどき とりあえずeplision_time
		static constexpr float gravity = 8.9f * 30;

		static int accuracy; //衝突解決の繰り返し回数 1～
	};

}
