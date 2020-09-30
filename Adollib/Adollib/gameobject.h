#pragma once

#include <list>
#include <memory>

#include "input.h"
#include "time.h"
#include "object.h"
#include "collider.h"
#include "component.h"
#include "transform.h"
#include "scene_list.h"
#include "material.h"
#include "ALP__manager.h"

namespace Adollib {

	class Gameobject : public object {
	private:
		struct ConstantBufferPerGO {
			DirectX::XMFLOAT4X4 world;
		};
		void update();
		ComPtr<ID3D11Buffer> world_cb; //WVP行列用バッファ
	public:
		bool no_material = false; //material情報を所持しているか

		std::string name = std::string("null"); //このgoの名前(検索用)

		std::shared_ptr<Material> material;

		std::vector <std::list<std::shared_ptr<Adollib::Collider>>::iterator> collider; //アタッチされているcolliderへのポインタ

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
			if (active == true)
			update();
			transform->local_orient = transform->local_orient.unit_vect();
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

		template<typename T>
		T* add_collider() {

			std::shared_ptr<T> newCom = std::make_shared<T>();
			Collider* pCom = dynamic_cast<Collider*>(newCom.get());
			if (pCom == nullptr)return nullptr;

			std::list<std::shared_ptr<Adollib::Collider>>::iterator C =  Rigitbody_manager::add_collider<T>(this_scene);
			(*C)->gameobject = this;
			collider.emplace_back(C);

			T* ret = dynamic_cast<T*>((*C).get());

			return ret;
		}

		void clear_collider() {
			for (int i = 0; i < collider.size();i++) {
				Rigitbody_manager::remove_collider(collider[i], this_scene);
			}
			collider.clear();
		}


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
			auto end = components.end();
			while (itr != end)
			{
				itr->get()->finalize();
				if (itr->get()) delete itr->get();
				itr = components.erase(itr);
			}
			components.clear();
		}

		//void removeGO() {

		//}

		//解放処理
		void destroy() {
			clearComponent();
			clear_collider();

			collider.clear();
		}
	};

}