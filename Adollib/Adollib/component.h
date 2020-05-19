#pragma once

#include "object.h"

#include <string>
#include <memory>

namespace Adollib
{
	// ****************************************************
	// gameobjectにアタッチされるすべてに対するベースクラス // TODO : インターフェースにしてもよいかも？
	// ****************************************************
	class Component : public object
	{
	public:

		// 更新処理の順を区別するためのフラグ
		bool is_camera_component = false;	// カメラならばtrue //一番最後に更新する?

	public:
		Component(const Component&) = delete;
		Component& operator=(const Component&) = delete;

		Component() = default;
		virtual ~Component() = default;


		// スクリプトをアタッチしたときに呼ぶ(transformの設定、これより前にアタッチしたコンポーネントまでしか取得できない)
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



		//QUESTION : なぜここに?
		// ==============================================================
		// このgameobjectにアタッチされているコンポーネントを取得する
		// ==============================================================
		// typename T	:	取得したいコンポーネント名
		// ==============================================================
		// 戻り値 T*		:	Tがあればそのポインタを、なければnullptrを返す
		// ==============================================================
		//template<typename T>
		//T* get_component() const
		//{
		//	for (auto&& com : gameobject->components)
		//	{
		//		if (typeid(T) == typeid(*com))
		//		{
		//			T* component = dynamic_cast<T*>(com.get());
		//			return component;
		//		}
		//	}
		//	return nullptr;
		//}
	};



}
