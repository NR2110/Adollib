#pragma once
#include "../Adollib/Scripts/Object/component.h"
#include "../Adollib/Scripts/Physics/collider.h"

#include "player.h"

namespace Adollib
{

	class Player_manager : public Component
	{
	private:
		Player* players[4];

		void add_player(int player_num, const Vector3& position, const Vector3& rotate, const Vector3& main_color, const Vector3& sub_color);

	public:
		void set_stage_manager_ptr(Stage_manager* m);

		void set_Tpause_and_set_transform(const Vector3& position, const Quaternion& orient);

		void set_moveable(bool is_moveable);

		//void add_player();
		//void delete_player(int num);


	public:
		void awake();

		// 所属するシーンの初期化時に一度だけ呼ばれる
		void start() override {};

		// 毎フレーム呼ばれる更新処理
		void update();

		// このスクリプトがアタッチされているGOのactiveSelfがtrueになった時呼ばれる
		void onEnable() override {};

		// このスクリプトがアタッチされているGOのactiveSelfがfalseになった時呼ばれる
		void onDisable() override {};

	};

}