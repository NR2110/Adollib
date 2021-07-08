#pragma once

#include "object.h"
#include "component.h"

//#include "../Shader/light_types.h"

#include "../Main/input.h"
#include "time.h"
#include <string>
#include <memory>
#include <vector>
#include <memory>

namespace Adollib
{
	struct POINTLIGHT;
	struct SPOTLIGHT;

	//カメラ用のコンポーネントクラス 継承不可!
	class Light_component final : public Component
	{
	public:
		std::vector <POINTLIGHT*> PointLight;
		std::vector <SPOTLIGHT*>  SpotLight;

		void set_dirLight(Vector3 dir, Vector3 color);

		void set_ambient(Vector3 amb);

		void set_pointLight(Vector3 pos, Vector3 color, float range);

		void set_spotLight(Vector3 pos, Vector3 color, Vector3 dir, float range, float near, float far);

	private:
		//managerに保存されている 自身へのitr
		std::list<Light_component*>::iterator this_itr;

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
