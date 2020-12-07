#pragma once

#include <list>
#include <memory>

#include "../Main/input.h"
#include "time.h"
#include "gameobject.h"
#include "component_camera.h"
#include "transform.h"
#include "../Scene/scene_list.h"

namespace Adollib {

	class Camera : public object {
	private:
		void update();
	public:
		std::string name = std::string("null"); //このgoの名前(検索用)

		std::list <std::shared_ptr<Component_camera>> components; //アタッチされているConponentのポインタ

		Scenelist this_scene = Scenelist::scene_null; //このgoのあるscene

		std::list<std::shared_ptr<Camera>>::iterator go_iterator; //自身へのイテレーター(いつ使うの?)

		float fov = 60.0f;
		float aspect = 1280 / 720.0f;
		float nearZ = 0.1f;
		float farZ = 1000000.0f;

		void update_imgui_P_to_C() override;

		void update_world_trans() override {
			transform->orientation = get_world_orientate();
			transform->position = get_world_position();
			transform->scale = get_world_scale();
		}


		//アタッチされたコンポーネントの処理
		void initialize();
		void render() {};

	private:

	public:
		Quaternion get_world_orientate() {
			if (pearent() != nullptr) return pearent()->transform->orientation * transform->local_orient;
			else return transform->local_orient;
		};
		Vector3 get_world_position() {
			if (pearent() != nullptr) return pearent()->transform->position + transform->local_pos;
			else return transform->local_pos;
		};
		Vector3 get_world_scale() {
			if (pearent() != nullptr) return pearent()->transform->scale * transform->local_scale;
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
			Component_camera* pCom = dynamic_cast<Component_camera*>(newCom);
			if (pCom != nullptr)
			{
				components.emplace_back(std::shared_ptr<Component_camera>(pCom));
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
			//components.clear();
		}

		//解放処理
		void destroy() {
			clearComponent();
		}
	};

}