// ReSharper disable All
#pragma once

#include <string>
#include <list>
#include <memory>

#include "../Math/math.h"
#include "../Main/input.h"
#include "time.h"
//#include "Gameobject.h"
#include "component.h"
#include "transform.h"
#include "gameobject_tags.h"
#include "../Scene/scene_list.h"
#include "../Renderer/material.h"

namespace Adollib {
	class Time;
	class Renderer_base;

	class Gameobject {
	public:
		Gameobject(const bool l_no_material, const Scenelist l_this_scene, std::list<Gameobject*>::iterator l_this_itr) :
			this_scene(l_this_scene),
			this_itr(l_this_itr)
		{};

	private:
		void update();
		void update_worldtrans() {
			transform->orientation = world_orientate();
			transform->position = world_position();
			transform->scale = world_scale();
		}

		std::shared_ptr<Time>& get_time_ptr();

		std::list <Component*> components; //アタッチされているComponentのポインタ

		Scenelist this_scene = Scenelist::scene_null; //このgoのあるscene

		std::list<Gameobject*>::iterator this_itr; //Gomanagerに保存された自身へのポインタ

		Gameobject* parent_ = nullptr; //親へのポインタ
		std::list<Gameobject*> children_; //子へのポインタ

	public:
		// このGOが存在するシーンを返す
		Scenelist get_scene() const { return this_scene; };

		// このGOのマテリアルへのポインタ cppから楽にアクセスできるようにここに置いている ここになくてもちゃんと動く
		//std::shared_ptr<Renderer> renderer = nullptr;
		Renderer_base* renderer = nullptr;

		std::shared_ptr<Transform> transform; //不本意なtransform

		std::string name; //このgoの名前

		bool is_active = true; //falseなら更新、描画を止める

		bool is_hierarchy = true;

		u_int tag = GO_tag::None; //このgoのtag(bit)


	public:
		// アタッチされたコンポーネントの処理
		void initialize();

		void update_imgui_toChildren();

		//自身のupdateしてから子のupdateを呼ぶ
		void update_to_children() {
			if (is_active == true)
				update();
			//transform->local_orient = transform->local_orient.unit_vect();
			std::for_each(children_.begin(), children_.end(), [](Gameobject* obj) {obj->update_to_children(); });
		}

		//自身のupdateしてから子のupdateを呼ぶ
		void update_world_trans_to_children() {
			if (is_active == true)
				update_worldtrans();
			//transform->local_orient = transform->local_orient.unit_vect();
			std::for_each(children_.begin(), children_.end(), [](Gameobject* obj) {obj->update_world_trans_to_children(); });
		}

	public:
		// goのworld空間上でのの姿勢を返す
		const Quaternion world_orientate() const {
			if (parent() != nullptr) {
				return transform->local_orient * parent()->transform->orientation;
			}
			else return transform->local_orient;
		};
		// goのworld空間上での座標を返す
		const Vector3 world_position() const {
			if (parent() != nullptr) {
				return parent()->transform->position + vector3_quatrotate(transform->local_pos * parent()->transform->scale, parent()->transform->orientation);
			}
			else return transform->local_pos;
		};
		// goのworld空間上でのscaleを返す
		const Vector3 world_scale() const {
			if (parent() != nullptr) {
				return parent()->transform->scale * transform->local_scale;
			}
			else return transform->local_scale;
		};

		// activeが変更されたときの処理を呼び出す
		void set_active(bool value) {
			if (is_active == value)return;
			is_active = value;
			if (value == false) {
				for (auto& comp : components)comp->onDisable();
			}
			else if (value == true) {
				for (auto& comp : components)comp->onEnable();

			}
		};

	public:

		Gameobject* parent() const { return parent_; }
		void set_parent(Gameobject* obj) { parent_ = obj; }
		const std::list<Gameobject*>* children() const { return &children_; }

		//一番の親を返す
		Gameobject* top_parent() {
			if (parent_ != nullptr) return parent_->top_parent();
			return this;
		};

		//thisの子にする
		void add_child(Gameobject* obj) {
			if (obj->parent() == this) return; //すでにこのGOが親として登録されていた
			if (obj->parent() != nullptr)obj->parent()->remove_child(obj);  //前の親から削除
			obj->set_parent(this);
			children_.emplace_back(obj);
		};

		//子からobjをremoveする
		void remove_child(Gameobject* obj) {
			auto itr = children_.begin();
			auto itr_end = children_.end();
			for (; itr != itr_end; ++itr) {
				if (*itr == obj) {
					children_.erase(itr);
					break;
				}
			}
		};

	public:

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
				pCom->transform = this->transform;
				pCom->gameobject = this;
				pCom->input = MonoInput::getInstancePtr();
				pCom->time = get_time_ptr();
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
					delete component;
					components.erase(itr);
					return true;
				}
			}
			return false;
		}
		bool removeComponent(Component* comp)
		{
			// 同一コンポーネントが存在する場合、removeする
			for (auto itr = components.begin(), itr_end = components.end(); itr != itr_end; ++itr)
			{
				if (comp->com_itr == itr) {
					(*itr)->finalize();
					delete (*itr);
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