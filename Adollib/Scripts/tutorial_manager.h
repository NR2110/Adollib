#pragma once

#include "../Adollib/Scripts/Object/gameobject_manager.h"
#include "../Adollib/Scripts/Object/component.h"

namespace Adollib {

	class Material;

	class Tutrial_manager : public Component{

	private:
		std::shared_ptr<Material> mat_tutrial_move = nullptr;
		std::shared_ptr<Material> mat_tutrial_camera = nullptr;
		std::shared_ptr<Material> mat_tutrial_hand = nullptr;


	public:



	public:
		void awake();

		// 所属するシーンの初期化時に一度だけ呼ばれる
		void start();

		// 毎フレーム呼ばれる更新処理
		void update();

		// このスクリプトがアタッチされているGOのactiveSelfがtrueになった時呼ばれる
		void onEnable() {};

		// このスクリプトがアタッチされているGOのactiveSelfがfalseになった時呼ばれる
		void onDisable() {};


	};


}