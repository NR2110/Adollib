#pragma once

#include "../Adollib/Scripts/Object/component_camera.h"
#include "../Adollib/Scripts/Physics/collider.h"

namespace Adollib
{
	class Camera : public Component
	{
	private:
		bool follow_player = false;
		float dis = 20; //距離
		float dis_buffer = 20; //距離のbuffer easing用
		float ray_timer = 0; //カメラがRayの影響でがくがくしないように Rayの影響を受けてからしばらくは自動的に戻らないように
		Vector3 pos_buffer; //座標のeasing用
		float pos_slop = 1; //カメラの座標の許容誤差

		bool is_lock_cursol = false;

		std::shared_ptr<Transform> player;
		Vector3 cursol_pos_save;
		Vector3 rotate_vec_save;

		Quaternion camera_rot;

	public:
		void awake();

		// 所属するシーンの初期化時に一度だけ呼ばれる
		void start();

		// 毎フレーム呼ばれる更新処理
		void update();

		// このスクリプトがアタッチされているGOのactiveSelfがtrueになった時呼ばれる
		void onEnable();

		// このスクリプトがアタッチされているGOのactiveSelfがfalseになった時呼ばれる
		void onDisable();

	};

}