#pragma once
#include "component.h"
#include "rigit_body.h"
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
		static std::map<Scenelist, std::vector<Adollib::Plane*>> RB_plane_s;
		static std::map<Scenelist, std::vector<Adollib::Box*>> RB_box_s;
		static std::map<Scenelist, std::vector<Adollib::Sphere*>> RB_sphere_s;

		static std::vector<Contacts::Contact_pair> pairs;

	public:
		//No_hit_tagに"all"があるとすべてのものに衝突しなくなるよ
		static void set_rigitbody(Adollib::Sphere* R, Gameobject* for_draw, std::string tag, std::vector<std::string> No_hit_tag = std::vector<std::string>(), Scenelist Sce = Scene::now_scene);
		//No_hit_tagに"all"があるとすべてのものに衝突しなくなるよ
		static void set_rigitbody(Adollib::Box* R,    Gameobject* for_draw, std::string tag, std::vector<std::string> No_hit_tag = std::vector<std::string>(), Scenelist Sce = Scene::now_scene);
		//No_hit_tagに"all"があるとすべてのものに衝突しなくなるよ
		static void set_rigitbody(Adollib::Plane* R,  Gameobject* for_draw, std::string tag, std::vector<std::string> No_hit_tag = std::vector<std::string>(), Scenelist Sce = Scene::now_scene);

	public:

		// 毎フレーム呼ばれる更新処理
		static void update(Scenelist Sce = Scene::now_scene);

		static void destroy(Scenelist Sce = Scene::now_scene);

	};

}