// ReSharper disable All
#pragma once

#include <list>
#include <memory>

#include "../Main/input.h"
#include "time.h"
#include "Object.h"
#include "component.h"
#include "transform.h"
#include "gameobject_tags.h"
#include "../Scene/scene_list.h"
#include "../Renderer/material.h"

namespace Adollib {

	class Gameobject : public Object {
	public:
		Gameobject(const bool l_no_material, const Scenelist l_this_scene, std::list<Gameobject*>::iterator l_this_itr) :
			this_scene(l_this_scene),
			this_itr(l_this_itr)
		{};

	private:
		void update() override;
		void update_worldtrans() override {
			transform->orientation = world_orientate();
			transform->position = world_position();
			transform->scale = world_scale();
		}

		std::list <Component*> components; //アタッチされているComponentのポインタ

		Scenelist this_scene = Scenelist::scene_null; //このgoのあるscene

		std::list<Gameobject*>::iterator this_itr; //Gomanagerに保存された自身へのポインタ

	public:
		// このGOが存在するシーンを返す
		Scenelist get_scene() const { return this_scene; };

		u_int tag = GO_tag::None; //このgoのtag(bit)

		std::string name; //このgoの名前

		// このGOのマテリアルへのポインタ cppから楽にアクセスできるようにここに置いている ここになくてもちゃんと動く
		std::shared_ptr<Material> material = nullptr;


	public:

		// アタッチされたコンポーネントの処理
		void initialize()override;

		void update_imgui_toChildren() override;
	private:


	public:
		// goのworld空間上でのの姿勢を返す
		const Quaternion world_orientate() const override {
			if (parent() != nullptr) {
				return transform->local_orient * parent()->transform->orientation;
			}
			else return transform->local_orient;
		};
		// goのworld空間上での座標を返す
		const Vector3 world_position() const override {
			if (parent() != nullptr) {
				return parent()->transform->position + vector3_quatrotate(transform->local_pos * parent()->transform->scale, parent()->transform->orientation);
			}
			else return transform->local_pos;
		};
		// goのworld空間上でのscaleを返す
		const Vector3 world_scale() const override {
			if (parent() != nullptr) {
				return parent()->transform->scale * transform->local_scale;
			}
			else return transform->local_scale;
		};

		// activeが変更されたときの処理を呼び出す
		void set_active(bool value) {
			if (active == value)return;
			active = value;
			if (value == false) {
				for (auto& comp : components)comp->onDisable();
			}
			else if (value == true) {
				for (auto& comp : components)comp->onEnable();

			}
		};


		// このGameObjectにコンポーネントをアタッチする
		template<typename T>
		T* addComponent()
		{
			// Componentクラスから派生したものかチェック
			static_assert(std::is_base_of<Component, T>::value == true, "template T must inherit Component");

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
			T* newCom = newD T();

			Component* pCom = dynamic_cast<Component*>(newCom);
			if (pCom != nullptr)
			{
				components.emplace_back(pCom);
				std::list<Component*>::iterator itr = components.end();
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

		template<typename T>
		T* findComponent(){
			// Componentクラスから派生したものかチェック
			static_assert(std::is_base_of<Component, T>::value == true, "template T must inherit Component");

			for (auto com : components) {
				T* comp = dynamic_cast<T*>(com) ;
				if (comp != nullptr) return comp;
			}

			return nullptr;
		}


		// このGameObjectにアタッチされているコンポーネントを削除する
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


		// このGameObjectにアタッチされているコンポーネントをすべて開放する
		void clearComponent() {
			//componentの終了処理を行う
			for (auto& comp : components)
			{
				comp->finalize();
				delete comp;
			}
			components.clear();
		}

		// 解放処理
		void destroy();
	};

}