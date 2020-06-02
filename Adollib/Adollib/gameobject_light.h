#pragma once

#include <list>
#include <memory>

#include "input.h"
#include "time.h"
#include "gameobject.h"
#include "component_light.h"
#include "transform.h"
#include "scene_list.h"
#include "light_types.h"

namespace Adollib {

	class Light : public object {
	private:
		void update();
	public:
		std::list<std::shared_ptr<object>> get_children() {		//すべての子を返す
			std::list<std::shared_ptr<object>>::iterator itr = children.begin();
			std::list<std::shared_ptr<object>> ret;

			for (int i = 0; i < children.size(); i++) {
				ret.splice(ret.end(), itr->get()->get_children());
				itr++;
			}
			return ret;
		};
		object* get_pearent() {		//一番の親を返す
			object* P = this;
			for (; P == nullptr;) {
				P = pearent;
			}
			return P;
		};
		void update_P_to_C() {
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
			//transform->orientation = get_world_orientate();
			//transform->position = get_world_position();
			//transform->scale = get_world_scale();
		}

		std::vector <std::shared_ptr<POINTLIGHT>> PointLight;
		std::vector <std::shared_ptr<SPOTLIGHT>>  SpotLight; 

		void set_dirLight(vector3 dir, vector3 color);

		void set_ambient(vector3 amb);

		void set_pointLight(vector3 pos, vector3 color, float range);

		void set_spotLight(vector3 pos, vector3 color, vector3 dir, float range, float near, float far);


		std::string name = std::string("null"); //このgoの名前(検索用)

		std::list <std::shared_ptr<Component_light>> components; //アタッチされているConponentのポインタ

		object* pearent = nullptr; //親へのポインタ
		std::list<std::shared_ptr<object>> children; //個へのポインタ

		bool active = true; //falseなら更新、描画を止める

		Scenelist this_scene = Scenelist::scene_null; //このgoのあるscene

		std::list<std::shared_ptr<Light>>::iterator go_iterator; //自身へのイテレーター(いつ使うの?)

		//アタッチされたコンポーネントの処理
		void initialize();
		void render() {};
	private:

	public:
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
			Component_light* pCom = dynamic_cast<Component_light*>(newCom);
			if (pCom != nullptr)
			{
				components.emplace_back(std::shared_ptr<Component_light>(pCom));
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