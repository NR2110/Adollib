#pragma once
#include "component.h"
#include "collider.h"
#include "scene_list.h"
#include "gameobject.h"
#include "scene.h"
#include <map>

#include "contact.h"

namespace Adollib
{
	class Rigitbody_manager
	{
	private:
		static std::unordered_map<Scenelist, std::vector<Adollib::Plane*>>  planes;
		static std::unordered_map<Scenelist, std::vector<Adollib::Box*>>    boxex;
		static std::unordered_map<Scenelist, std::vector<Adollib::Sphere*>> spheres;
		static std::unordered_map<Scenelist, std::vector<Adollib::Meshcoll*>> meshcolls;

		static std::vector<Contacts::Contact_pair> pairs;

	public:
		//No_hit_tagに"all"があるとすべてのものに衝突しなくなるよ
		static void add_collider(Adollib::Sphere* R, Gameobject* for_draw, std::string tag, std::vector<std::string> No_hit_tag = std::vector<std::string>(), Scenelist Sce = Scene::now_scene);
		//No_hit_tagに"all"があるとすべてのものに衝突しなくなるよ
		static void add_collider(Adollib::Box* R,    Gameobject* for_draw, std::string tag, std::vector<std::string> No_hit_tag = std::vector<std::string>(), Scenelist Sce = Scene::now_scene);
		//No_hit_tagに"all"があるとすべてのものに衝突しなくなるよ
		static void add_collider(Adollib::Plane* R,  Gameobject* for_draw, std::string tag, std::vector<std::string> No_hit_tag = std::vector<std::string>(), Scenelist Sce = Scene::now_scene);
		//No_hit_tagに"all"があるとすべてのものに衝突しなくなるよ
		static void add_collider(Adollib::Meshcoll* R, Gameobject* for_draw, std::string tag, std::vector<std::string> No_hit_tag = std::vector<std::string>(), Scenelist Sce = Scene::now_scene);




	public:

		// 毎フレーム呼ばれる更新処理
		static bool update(Scenelist Sce = Scene::now_scene);

		static void destroy(Scenelist Sce = Scene::now_scene);

	};

}