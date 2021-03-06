#pragma once

#include <list>
#include <memory>

#include "../Main/input.h"
#include "time.h"
#include "gameobject.h"
#include "component_light.h"
#include "transform.h"
#include "../Scene/scene_list.h"
#include "../Shader/light_types.h"

namespace Adollib {

	class Light : public object {
	public:
		Light(Scenelist l_this_scene, std::list<Light*>::iterator l_this_itr) :
			this_scene(l_this_scene),
			this_itr(l_this_itr)
		{};

	private:
		void update();


		std::list <std::shared_ptr<Component_light>> components; //アタッチされているConponentのポインタ

		std::list<Light*>::iterator this_itr; //自身へのイテレーター(いつ使うの?)B

		Scenelist this_scene = Scenelist::scene_null; //このgoのあるscene

	public:
		std::string name = std::string("null"); //このgoの名前(検索用)

		void update_imgui_toChildren() override;

		void update_worldtrans() {
			//transform->orientation = get_world_orientate();
			//transform->position = get_world_position();
			//transform->scale = get_world_scale();
		}

		std::vector <std::shared_ptr<POINTLIGHT>> PointLight;
		std::vector <std::shared_ptr<SPOTLIGHT>>  SpotLight;

		void set_dirLight(Vector3 dir, Vector3 color);

		void set_ambient(Vector3 amb);

		void set_pointLight(Vector3 pos, Vector3 color, float range);

		void set_spotLight(Vector3 pos, Vector3 color, Vector3 dir, float range, float near, float far);

		//アタッチされたコンポーネントの処理
		void initialize();
		void render() {};
	private:

	public:
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
			T* newCom = newD T();

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
				if (itr->get()) delete itr->get();
				itr = components.erase(itr);
			}
			//components.clear();
		}

		//解放処理
		void destroy();
	};

}