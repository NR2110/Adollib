#pragma once

#include <list>
#include <memory>

#include "gameobject.h"
#include "component.h"
#include "transform.h"
#include "scene_list.h"
#include "light_types.h"

namespace Adollib {

	class Light : public object {
	public:
		std::vector <std::shared_ptr<POINTLIGHT>> PointLight;
		std::vector <std::shared_ptr<SPOTLIGHT>>  SpotLight;

		void set_dirLight(DirectX::XMFLOAT3 dir, DirectX::XMFLOAT3 color);

		void set_ambient(DirectX::XMFLOAT3 amb);

		void set_pointLight(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 color, float range);

		void set_spotLight(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 color, DirectX::XMFLOAT3 dir, float range, float near, float far);


		std::string name = std::string("null"); //このgoの名前(検索用)

		std::list <std::shared_ptr<Component>> components; //アタッチされているConponentのポインタ

		Gameobject* pearent = nullptr; //親へのポインタ
		std::list<std::shared_ptr<Gameobject>> children; //個へのポインタ

		bool active = true; //falseなら更新、描画を止める

		Scenelist this_scene = Scenelist::scene_null; //このgoのあるscene

		std::list<std::shared_ptr<Light>>::iterator go_iterator; //自身へのイテレーター(いつ使うの?)

		//アタッチされたコンポーネントの処理
		void initialize();
		void update();
		void render() {};
	private:

	public:

	};


}