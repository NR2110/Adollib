#pragma once

#include <list>
#include <memory>

#include "gameobject.h"
#include "component.h"
#include "transform.h"
#include "scene_list.h"

namespace Adollib {

	class Camera : public object {
	public:
		std::string name = std::string("null"); //このgoの名前(検索用)

		Transfome* transform = nullptr; //描画用の情報

		std::list <std::shared_ptr<Component>> components; //アタッチされているConponentのポインタ

		Gameobject* pearent = nullptr; //親へのポインタ
		std::list<std::shared_ptr<Gameobject>> children; //個へのポインタ

		bool active = true; //falseなら更新、描画を止める

		Scenelist this_scene = Scenelist::scene_null; //このgoのあるscene

		std::list<std::shared_ptr<Camera>>::iterator go_iterator; //自身へのイテレーター(いつ使うの?)

		//アタッチされたコンポーネントの処理
		void initialize();
		void update();
		void render() {};
	private:

	public:
		quaternion get_world_orientate() {
			if (pearent != nullptr) return pearent->transform->prientation * transform->prientation;
			else return transform->prientation;
		};
		vector3 get_world_position() {
			if (pearent != nullptr) return pearent->transform->position + transform->position;
			else return transform->position;
		};

	};

}