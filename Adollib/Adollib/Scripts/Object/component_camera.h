#pragma once

#include "Object.h"
#include "component.h"
#include "../Main/input.h"
#include "time.h"
#include <string>
#include <memory>

namespace Adollib
{
	//カメラ用のコンポーネントクラス 継承不可!
	class Camera_component final : public Component
	{
	public:
		float fov = 60.0f;
		float aspect = 1280 / 720.0f;
		float nearZ = 0.1f;
		float farZ = 1000000.0f;


	private:
		//managerに保存されている 自身へのitr
		std::list<Camera_component*>::iterator this_itr;

	public:

		// addComponentされたときに呼ばれる
		void awake() override;

		// 所属するシーンの初期化時に一度だけ呼ばれる
		virtual void start() override {};

		// Hierarchyの表示(Imguiの関数 Imgui::begin,Imgui::endはいらない)
		virtual void Update_hierarchy() override;

		// 毎フレーム呼ばれる更新処理
		virtual void update() override {};

		// このスクリプトがアタッチされているGOのactiveSelfがtrueになった時呼ばれる(GO生成時にスクリプトが有効な時も呼ばれる)
		virtual void onEnable() override {};

		// このスクリプトがアタッチされているGOのactiveSelfがfalseになった時呼ばれる
		virtual void onDisable() override {};

		// removeComponent()、clear()時に呼ぶ
		void finalize() override;

	};



};
