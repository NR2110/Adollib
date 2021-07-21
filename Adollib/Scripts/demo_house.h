#pragma once
#include "../Adollib/Scripts/Object/component.h"
#include "../Adollib/Scripts/Physics/collider.h"

namespace Adollib
{

	class Demo_house : public Component
	{
	private:

		Gameobject* camera;

	public:
		Gameobject* set_tree(Vector3 pos, Vector3 scale, Vector3 rotate, Vector3 color = Vector3(1, 1, 1));

		Gameobject* set_sphere(Collider*& coll, Vector3 pos, float r, Vector3 color = Vector3(1, 1, 1));
		Gameobject* set_box(Vector3 pos, Vector3 size, Vector3 rotate, Vector3 color = Vector3(1, 1, 1));
		Gameobject* set_capsule(Vector3 pos, float r, float length, Vector3 rotate, Vector3 color = Vector3(1, 1, 1));
		Gameobject* set_plane(Vector3 pos, Vector3 normal, Vector3 color = Vector3(1, 1, 1));

		Gameobject* set_meshbox(Vector3 pos, Vector3 size, Vector3 rotate, Vector3 color = Vector3(1, 1, 1));

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