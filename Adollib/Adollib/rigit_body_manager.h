#pragma once
#include "component.h"
#include "rigit_body.h"
#include "scene_list.h"
#include "gameobject.h"
#include "scene.h"
#include <map>

namespace Adollib
{
	class Rigitbody_manager
	{
	private:
		struct RB_base {
			Gameobject* pearent;
			std::string tag;
			std::vector<std::string> No_hit_tag;
		};
		struct RB_sphere : public RB_base {
			Adollib::Sphere* R;
		};
		struct RB_box : public RB_base {
			Adollib::Box* R;
		};
		struct RB_plane : public RB_base {
			Adollib::Plane* R;
		};
		static std::map<Scenelist, std::vector<RB_sphere>> RB_sphere_s;
		static std::map<Scenelist, std::vector<RB_box>> RB_box_s;
		static std::map<Scenelist, std::vector<RB_plane>> RB_plane_s;

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