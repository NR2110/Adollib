#pragma once

#include <list>
#include <memory>

#include "object.h"
#include "component.h"
#include "transform.h"
#include "scene_list.h"
#include "material.h"

namespace Adollib {
	class Gameobject : public object {
	private:
		struct ConstantBufferPerGO {
			DirectX::XMMATRIX world;
		};

		ComPtr<ID3D11Buffer> world_cb; //WVP行列用バッファ

	public:
		std::string name = std::string("null"); //このgoの名前(検索用)

		Transfome* transform = nullptr; //描画用の情報

		Material* material = nullptr;

		std::list < std::shared_ptr< RigitBody_Transform>> collider; //アタッチされているcollider

		std::list <std::shared_ptr<Component>> components; //アタッチされているConponentのポインタ

		Gameobject* pearent = nullptr; //親へのポインタ
		std::list<std::shared_ptr<Gameobject>> children; //個へのポインタ

		bool active = true; //falseなら更新、描画を止める

		Scenelist this_scene = Scenelist::scene_null; //このgoのあるscene

		std::list<std::shared_ptr<Gameobject>>::iterator go_iterator; //自身へのイテレーター(いつ使うの?)

		//アタッチされたコンポーネントの処理
		void initialize();
		void update();
		void render();
	private:


	public:
		//このgoのworld変換行列を返す
		matrix get_word_matrix() {
			matrix WorldMatrix;
			if (pearent != nullptr) {
				WorldMatrix = (transform->prientation).get_rotate_matrix();
				WorldMatrix._41 = transform->position.x;
				WorldMatrix._42 = transform->position.y;
				WorldMatrix._43 = transform->position.z;
			}
			else {
				WorldMatrix = (transform->prientation * pearent->transform->prientation).get_rotate_matrix();
				WorldMatrix._41 = (transform->position + pearent->transform->position).x;
				WorldMatrix._42 = (transform->position + pearent->transform->position).y;
				WorldMatrix._43 = (transform->position + pearent->transform->position).z;
			}
			return WorldMatrix;
		};

		//goのworld空間上でのの姿勢を返す
		quaternion get_world_orientate() {
			if (pearent != nullptr) return pearent->transform->prientation * transform->prientation;
			else return transform->prientation;
		};
		//goのworld空間上での座標を返す
		vector3 get_world_position() {
			if (pearent != nullptr) return pearent->transform->position + transform->position;
			else return transform->position;
		};

		//:::::::::::::::::::::::::
		// pos : 描画座標からの相対座標
		// r : 半径
		//:::::::::::::::::::::::::
		void set_collider_sphere(vector3 pos, float r);

		//:::::::::::::::::::::::::
		// pos : 描画座標からの相対座標
		// size : 描画回転からの x,y,z の相対half_size
		// rotate : 描画回転からの相対回転
		//:::::::::::::::::::::::::
		void set_collider_box(vector3 pos, vector3 size, quaternion orient);

		//:::::::::::::::::::::::::
		// pos : 相対座標
		// normal : 相対法線
		//:::::::::::::::::::::::::
		void set_collider_plane(vector3 pos, vector3 normal);

	};

}