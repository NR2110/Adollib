#pragma once
#include "component.h"
#include "collider.h"
#include "scene_list.h"
#include "scene.h"
#include <map>

#include "contact.h"

namespace Adollib
{

	class Rigitbody_manager
	{
	private:
		static std::unordered_map<Scenelist, std::list<std::shared_ptr<Adollib::Collider>>>  colliders;

		static std::vector<Contacts::Contact_pair> pairs;

	public:
		//No_hit_tagに"all"があるとすべてのものに衝突しなくなるよ
		template<typename T>
		static T* add_collider(Scenelist Sce) {

			std::shared_ptr<T> newCom = std::make_shared<T>();
			Collider* pCom = dynamic_cast<Collider*>(newCom.get());

			if (pCom == nullptr)return nullptr;

			colliders[Sce].emplace_back(newCom);

			return newCom.get();
		}

		static void remove_collider(Scenelist Sce) {

			colliders[Sce].emplace_back(newCom);

			return newCom.get();
		}

		static void remove_collider(Adollib::Collider* R, Scenelist Sce = Scene::now_scene);
		static void remove_collider_perGO(Adollib::Gameobject* GO, Scenelist Sce = Scene::now_scene);

	public:

		// 毎フレーム呼ばれる更新処理
		static bool update(Scenelist Sce = Scene::now_scene);

		static void destroy(Scenelist Sce = Scene::now_scene);

	};

}