#pragma once

#include "../Adollib/Scripts/Object/component_camera.h"
#include "../Adollib/Scripts/Physics/collider.h"

namespace Adollib
{
	class Input_changer;


	class Camera : public Component
	{
	private:
		bool follow_player = false;
		float dis = 20; //距離
		float dis_buffer = 20; //距離のbuffer easing用
		float ray_timer = 0; //カメラがRayの影響でがくがくしないように Rayの影響を受けてからしばらくは自動的に戻らないように
		Vector3 pos_buffer; //座標のeasing用
		float pos_slop = 1; //カメラの座標の許容誤差

		Vector2 max_rotate = Vector2(-50, 60); //カメラの上下の最大角度
		//Vector2 max_rotate_moving = Vector2(-15, 15); //移動中のカメラの上下の最大角度
		float rotate_min_pow_bufer = 1;
		float rotate_min_pow = 0.3f;
		//Vector2 max_rotate_buffer = Vector2(-50, 60); //カメラの最大角度のイージングのbuffer

		std::shared_ptr<Transform> player;
		Input_changer* input_changer;
		Vector3 rotate_vec_save;

		Quaternion camera_rot_goal; //Easing用

		float rotate_speed = 0.5f;
		float linear_speed = 30;

	public:
		Quaternion camera_rot; //cameraの向き

		void set_player_transformptr(std::shared_ptr<Transform> p) { player = p; };
		void set_input_changerptr(Input_changer* p) { input_changer = p; };

	public:
		void awake() override;

		// 所属するシーンの初期化時に一度だけ呼ばれる
		void start() override;

		// 毎フレーム呼ばれる更新処理
		void update() override;

		// このスクリプトがアタッチされているGOのactiveSelfがtrueになった時呼ばれる
		void onEnable() override;

		// このスクリプトがアタッチされているGOのactiveSelfがfalseになった時呼ばれる
		void onDisable() override;

	};

}