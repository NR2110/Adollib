#pragma once

#include "../Adollib/Scripts/Object/component_camera.h"
#include "../Adollib/Scripts/Physics/collider.h"

#include "title_state_base.h"

namespace Adollib
{
	class Title_state_manager;

	class Title_camera : public Component, public Title_state_base
	{
	private:
		float rotate_speed = 0.5f;
		float linear_speed = 30;

		Quaternion camera_rot; //cameraの向き
		Vector2 max_rotate = Vector2(-50, 60); //カメラの上下の最大角度
		Vector3 rotate_vec_save;

		Vector2 cursol_pos_save;

		Vector3 start_pos;
		Quaternion start_rotate;

	private:
		Title_state_manager* title_state_manager = nullptr;
		std::shared_ptr<Transform> player;

	public:

		void Set_start_transform(const Vector3& l_start_pos, const Quaternion& l_start_rotate) {
			start_pos = l_start_pos;
			start_rotate = l_start_rotate;
			transform->local_pos = l_start_pos;
			transform->local_orient = l_start_rotate;
		}

	public:
		void start_state(Title_state_B state) override;

		void select_state(Title_state_B state) override;


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