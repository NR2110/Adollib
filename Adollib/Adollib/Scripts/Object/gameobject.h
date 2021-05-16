#pragma once

#include <list>
#include <memory>

#include "../Main/input.h"
#include "time.h"
#include "object.h"
#include "component.h"
#include "transform.h"
#include "gameobject_tags.h"
#include "../Scene/scene_list.h"
#include "../Mesh/material.h"

namespace Adollib {

	class Gameobject : public object {
	private:
		void update();
		void update_worldtrans() override {
			transform->orientation = get_world_orientate();
			transform->position = get_world_position();
			transform->scale = get_world_scale();
		}

		ComPtr<ID3D11Buffer> world_cb; //WVP行列用バッファ
	public:
		bool no_material = false; //material情報を所持しているか

		u_int tag = GO_tag::None; //このgoのtag(bit)

		std::string name; //このgoの名前

		std::shared_ptr<Material> material; //このGOのマテリアル

		std::list <std::shared_ptr<Component>> components; //アタッチされているConponentのポインタ

		Scenelist this_scene = Scenelist::scene_null; //このgoのあるscene

		std::list<std::shared_ptr<object>>::iterator go_iterator; //自身へのイテレーター(いつ使うの?)

		//アタッチされたコンポーネントの処理
		void initialize()override;
		void render()override;

		void update_imgui_toChildren() override;
	private:


	public:
		//goのworld空間上でのの姿勢を返す
		Quaternion get_world_orientate()override {
			if (pearent() != nullptr) {
				return transform->local_orient * pearent()->transform->orientation ;
			}
			else return transform->local_orient;
		};
		//goのworld空間上での座標を返す
		Vector3 get_world_position()override {
			if (pearent() != nullptr) {
				return pearent()->transform->position + vector3_quatrotate(transform->local_pos * pearent()->transform->scale, pearent()->transform->orientation);
			}
			else return transform->local_pos;
		};
		//goのworld空間上でのscaleを返す
		Vector3 get_world_scale() override {
			if (pearent() != nullptr) {
				return pearent()->transform->scale * transform->local_scale;
			}
			else return transform->local_scale;
		};

		//activeが変更されたときの処理を呼び出す
		void set_active(bool value) {
			if (active == value)return;
			active = value;
			if (value == false) {
				auto itr = components.begin();
				auto end = components.end();
				while (itr != end)
				{
					itr->get()->onDisable();
					itr++;
				}
			}
			else if (value == true) {
				auto itr = components.begin();
				auto end = components.end();
				while (itr != end)
				{
					itr->get()->onEnable();
					itr++;
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
			T* newCom = DBG_NEW T();

			// Componentクラスから派生したものかチェック
			Component* pCom = dynamic_cast<Component*>(newCom);
			if (pCom != nullptr)
			{
				components.emplace_back(std::shared_ptr<Component>(pCom));
				std::list <std::shared_ptr<Component>>::iterator itr = components.end();
				itr--;
				pCom->com_itr = itr;
				pCom->transform = this->transform.get();
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
			auto itr_save = itr;
			auto end = components.end();
			while (itr != end)
			{
				itr->get()->finalize();
				itr_save = itr;
				itr++;
				components.erase(itr_save);
			}
			components.clear();
		}

		//void removeGO() {

		//}

		//解放処理
		void destroy() {
			clearComponent();
		}
	};

}