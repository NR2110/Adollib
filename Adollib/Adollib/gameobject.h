#pragma once

#include <list>
#include <memory>

#include "input.h"
#include "time.h"
#include "object.h"
#include "rigit_body.h"
#include "component.h"
#include "transform.h"
#include "scene_list.h"
#include "material.h"

namespace Adollib {
	class Gameobject : public object {
	private:
		struct ConstantBufferPerGO {
			DirectX::XMFLOAT4X4 world;
		};
		void update();
		ComPtr<ID3D11Buffer> world_cb; //WVP行列用バッファ
	public:

		std::string name = std::string("null"); //このgoの名前(検索用)

		Material* material = nullptr;

		std::list < std::shared_ptr<Rigitbody*>> collider; //アタッチされているcolliderへのポインタ

		std::list <std::shared_ptr<Component>> components; //アタッチされているConponentのポインタ

		object* pearent = nullptr; //親へのポインタ
		object* get_pearent() {		//一番の親を返す
			object* P = this;
			for (; P == nullptr;) {
				P = pearent;
			}
			return P;
		};

		std::list<std::shared_ptr<object>> children; //個へのポインタ
		std::list<std::shared_ptr<object>> get_children() {		//すべての子を返す
			std::list<std::shared_ptr<object>>::iterator itr = children.begin();
			std::list<std::shared_ptr<object>> ret;

			for (int i = 0; i < children.size(); i++) {
				ret.splice(ret.end(), itr->get()->get_children());
				itr++;
			}
			return ret;
		};

		bool active = true; //falseなら更新、描画を止める

		Scenelist this_scene = Scenelist::scene_null; //このgoのあるscene

		std::list<std::shared_ptr<object>>::iterator go_iterator; //自身へのイテレーター(いつ使うの?)

		//アタッチされたコンポーネントの処理
		void initialize();
		void render();
		void update_P_to_C() {
			update();
			std::list<std::shared_ptr<object>>::iterator itr = children.begin();
			std::list<std::shared_ptr<object>>::iterator itr_end = children.end();
			for (; itr != itr_end;) {
				itr->get()->update_P_to_C();
				itr++;
			}
		}
		void update_world_trans() {
			transform->orientation = get_world_orientate();
			transform->position = get_world_position();
			transform->scale = get_world_scale();
		}
	private:


	public:
		//goのworld空間上でのの姿勢を返す
		quaternion get_world_orientate() {
			if (pearent != nullptr) return pearent->transform->orientation * transform->local_orient;
			else return transform->local_orient;
		};
		//goのworld空間上での座標を返す
		vector3 get_world_position() {
			if (pearent != nullptr) return pearent->transform->position + transform->local_pos;
			else return transform->local_pos;
		};
		//goのworld空間上でのscaleを返す
		vector3 get_world_scale() {
			if (pearent != nullptr) return pearent->transform->scale * transform->local_scale;
			else return transform->local_scale;
		};



		void set_collider(Sphere* S, std::string tag, std::vector<std::string> no_hit_tag);
		void set_collider(Box* B,    std::string tag, std::vector<std::string> no_hit_tag);
		void set_collider(Plane* P,  std::string tag, std::vector<std::string> no_hit_tag);

		//:::::::::::::::::::::::::
		// pos : 描画座標からの相対座標
		// r : 半径
		//:::::::::::::::::::::::::
		void add_collider_sphere(
			vector3 pos = vector3(0, 0, 0), 
			float r = 1, 
			float density = 1, 
			std::string tag = std::string("Sphere"), 
			std::vector<std::string> no_hit_tag = std::vector<std::string>()
		);

		//:::::::::::::::::::::::::
		// pos : 描画座標からの相対座標
		// size : 描画回転からの x,y,z の相対half_size
		// rotate : 描画回転からの相対回転
		//:::::::::::::::::::::::::
		void add_collider_box(
			vector3 pos = vector3(0, 0, 0),
			vector3 size = vector3(1, 1, 1), 
			quaternion orient = quaternion_identity(),
			float density = 1,
			std::string tag = std::string("Box"),
			std::vector<std::string> no_hit_tag = std::vector<std::string>()
		);

		//:::::::::::::::::::::::::
		// pos : 相対座標
		// normal : 相対法線
		//:::::::::::::::::::::::::
		void add_collider_plane(
			vector3 pos = vector3(0, 0, 0),
			vector3 normal = vector3(0, 1, 0),
			float density = 1, 
			std::string tag = std::string("Plane"),
			std::vector<std::string> no_hit_tag = std::vector<std::string>()
		);


		//activeが変更されたときの処理を呼び出す
		void set_active(bool value) {
			if (active == value)return;
			if (value == false) {
				auto itr = components.begin();
				auto end = components.end();
				while (itr != end)
				{
					itr->get()->onDisable();
				}
			}
			else if (value == true) {
				auto itr = components.begin();
				auto end = components.end();
				while (itr != end)
				{
					itr->get()->onEnable();
				}
			}
		};
		// ==============================================================
		// このGameObjectにコンポーネントをアタッチする
		// ==============================================================
		// typename T	:	追加するコンポーネント名
		// ==============================================================
		// 戻り値 T*		:	追加したコンポーネントを返す。既にあるか、Componentから派生していない場合nullptr
		// ==============================================================
		template<typename T>
		T* addComponent()
		{
			// 既に同一コンポーネントが存在する場合、Addしない
			for (auto&& com : components)
			{
				//if (dynamic_cast<T*>(com) != nullptr)
				if (typeid(T) == typeid(*com))
				{
					//static_assert(0, "ERROR");
					return nullptr;
				}
			}

			//std::shared_ptr<T> newCom = std::make_shared<T>();
			T* newCom = new T();

			// Componentクラスから派生したものかチェック
			Component* pCom = dynamic_cast<Component*>(newCom);
			if (pCom != nullptr)
			{
				components.emplace_back(std::shared_ptr<Component>(pCom));
				pCom->transform = this->transform;
				pCom->gameobject = this;
				pCom->input = MonoInput::getInstancePtr();
				pCom->time = Time::getInstancePtr();
				pCom->awake();		// 初期化処理を定義してあるコンポーネントならばそれを呼ぶ
				return newCom;
			}
			return nullptr;
		}

		// ==============================================================
		// このGameObjectにアタッチされているコンポーネントを削除する
		// ==============================================================
		// typename T	:	削除したいコンポーネント名
		// ==============================================================
		// 戻り値 bool	:	削除したらtrue、していないならfalse
		// ==============================================================
		template<typename T>
		bool removeComponent()
		{
			// 同一コンポーネントが存在する場合、removeする
			for (auto itr = components.begin(), itr_end = components.end(); itr != itr_end; ++itr)
			{
				if (typeid(T*) == typeid(*itr))
				{
					T* component = dynamic_cast<T*>(*itr);
					//delete component;
					component->finalize();
					components.erase(itr);
					return true;
				}
			}
			return false;
		}


		// ==============================================================
		// このGameObjectにアタッチされているコンポーネントをすべて開放する
		// ==============================================================
		void clearComponent() {
			auto itr = components.begin();
			auto end = components.end();
			while (itr != end)
			{
				itr->get()->finalize();
				itr = components.erase(itr);
			}
			//components.clear();
		}


	};

}