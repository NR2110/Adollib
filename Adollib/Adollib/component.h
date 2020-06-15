#pragma once

#include "object.h"
#include "input.h"
#include "time.h"
#include <string>
#include <memory>

namespace Adollib
{
	class Gameobject;
	class Transfome;

	// ****************************************************
	// gameobjectにアタッチされるすべてに対するベースクラス // TODO : インターフェースにしてもよいかも？
	// ****************************************************
	class Component 
	{
	public:
		Gameobject* gameobject;	// このコンポーネントがアタッチされているGameObject
		Transfome*  transform;	// GameObjectのTransformへのポインタ

		MonoInput* input = nullptr;
		Time* time = nullptr;

	public:
		Component(const Component&) = delete;
		Component& operator=(const Component&) = delete;

		Component() = default;
		virtual ~Component() = default;


		// addComponentされたときに呼ばれる
		virtual void awake() {};

		// 所属するシーンの初期化時に一度だけ呼ばれる
		virtual void start() {};

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
