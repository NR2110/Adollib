#pragma once
#include "../Object/component.h"
#include "collider.h"
#include "../Scene/scene_list.h"
#include "../Scene/scene.h"

#include "ALP_collider.h"
#include "ALP_physics.h"
#include "ALP_shape.h"

#include <map>

#include "contact.h"

namespace Adollib
{

	class Rigitbody_manager
	{
	private:
		static int flame_count;

		static std::unordered_map<Scenelist, std::list<Adollib::Collider*>>  colliders;

		static std::vector<Adollib::physics_function::Collider> ALP_colliders;
		static std::vector<Adollib::physics_function::Physics> ALP_physicses;
		static std::vector<Adollib::physics_function::Shape> ALP_shapes;


		static std::vector<Contacts::Contact_pair> pairs;

	public:
		//No_hit_tagに"all"があるとすべてのものに衝突しなくなるよ
		static std::list<Adollib::Collider*>::iterator add_collider(Collider* coll, Scenelist Sce = Scene::now_scene) {

			colliders[Sce].emplace_back(coll);

			auto ret = colliders[Sce].end();
			ret--;

			(*ret)->coll_itr = ret;

			return ret;
		}

		static void remove_collider(std::list<Adollib::Collider*>::iterator itr, Scenelist Sce = Scene::now_scene) {
			colliders[Sce].erase(itr);
		}

		static void remove_collider(Adollib::Collider* R, Scenelist Sce = Scene::now_scene) {
			colliders[Sce].erase(R->coll_itr);
		}
		static void remove_collider_perGO(Adollib::Gameobject* GO, Scenelist Sce = Scene::now_scene);

	public:

		// 毎フレーム呼ばれる更新処理
		static bool update(Scenelist Sce = Scene::now_scene);

		static void destroy(Scenelist Sce = Scene::now_scene);

	};

}