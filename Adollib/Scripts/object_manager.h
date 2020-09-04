#pragma once
#include "../Adollib/component.h"
#include "../Adollib/collider.h"

namespace Adollib
{

	class object_manager : public Component
	{
	private:
		std::vector<Box*> boxes;
		std::vector<Box>  st_box;

	public:
		Gameobject* set_sphere(vector3 pos, float r, vector3 color = vector3(1, 1, 1));
		Gameobject* set_box(vector3 pos, vector3 size, vector3 rotate, vector3 color = vector3(1, 1, 1));
		Gameobject* set_plane(vector3 pos, vector3 normal, vector3 color = vector3(1, 1, 1));

		Gameobject* set_fall_sphere(vector3 pos, float r, vector3 color = vector3(1, 1, 1));
		Gameobject* set_fall_box(vector3 pos, vector3 size, vector3 rotate, vector3 color = vector3(1, 1, 1));
		Gameobject* set_fall_plane(vector3 pos, vector3 normal, vector3 color = vector3(1, 1, 1));

		Gameobject* set_nohit_sphere(vector3 pos, float r, vector3 color = vector3(1, 1, 1));
		Gameobject* set_nohit_box(vector3 pos, vector3 size, vector3 rotate, vector3 color = vector3(1, 1, 1));
		Gameobject* set_nohit_plane(vector3 pos, vector3 normal, vector3 color = vector3(1, 1, 1));

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