#pragma once

#include "Object.h"
#include "../Main/input.h"
#include "time.h"
#include <string>
#include <memory>

namespace Adollib
{
	class Gameobject;
	class Transform;

	// gameobjectにアタッチされるベースクラス
	class Component
	{
	public:
		std::list<Component*>::iterator com_itr; //自身へのイテレータ

		Gameobject* gameobject = nullptr;	// このコンポーネントがアタッチされているGameObject
		Transform*  transform = nullptr;	// GameObjectのTransformへのポインタ

		MonoInput* input = nullptr;
		std::shared_ptr<Time> time = nullptr;

		std::string name; //Hierarchyに表示する名前

	public:
		Component(const Component&) = delete;
		Component& operator=(const Component&) = delete;

		Component() = default;
		virtual ~Component() = default;


		// addComponentされたときに呼ばれる
		virtual void awake() {};

		// 所属するシーンの初期化時に一度だけ呼ばれる
		virtual void start() {};

		// Hierarchyの表示(Imguiの関数 Imgui::begin,Imgui::endはいらない)
		virtual void Update_hierarchy() {};

		// 毎フレーム呼ばれる更新処理
		virtual void update() {};

		// このスクリプトがアタッチされているGOのactiveSelfがtrueになった時呼ばれる(GO生成時にスクリプトが有効な時も呼ばれる)
		virtual void onEnable() {};

		// このスクリプトがアタッチされているGOのactiveSelfがfalseになった時呼ばれる
		virtual void onDisable() {};

		// removeComponent()、clear()時に呼ぶ
		virtual void finalize() {};

	};



}
