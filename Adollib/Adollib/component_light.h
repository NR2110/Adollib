#pragma once

#include "object.h"
#include "input.h"
#include "time.h"
#include <string>
#include <memory>

//TODO : こんなコンポーネント作りたくない 継承使ってなくす
namespace Adollib
{
	class Light;
	class Transfome;

	// ****************************************************
	// gameobject_cameraにアタッチされるすべてに対するベースクラス // TODO : インターフェースにしてもよいかも？
	// ****************************************************
	class Component_light 
	{
	public:
		Light* gameobject;	// このコンポーネントがアタッチされているGameObject
		Transfome* transform;	// GameObjectのTransformへのポインタ

		MonoInput* input = nullptr;
		Time* time = nullptr;

	public:
		Component_light(const Component_light&) = delete;
		Component_light& operator=(const Component_light&) = delete;

		Component_light() = default;
		virtual ~Component_light() = default;



		// addComponentされたときに呼ばれる
		virtual void awake() {};

		// 所属するシーンの初期化時に一度だけ呼ばれる
		virtual void start() {};

		// 毎フレーム呼ばれる更新処理
		virtual void update() {};

		// 毎フレーム、update()後に呼ばれる更新処理
		virtual void lateUpdate() {};

		// このスクリプトがアタッチされているGOのactiveSelfがtrueになった時呼ばれる(GO生成時にスクリプトが有効な時も呼ばれる)
		virtual void onEnable() {};

		// このスクリプトがアタッチされているGOのactiveSelfがfalseになった時呼ばれる
		virtual void onDisable() {};

		// removeComponent()、clear()時に呼ぶ
		virtual void finalize() {};

	};



}
