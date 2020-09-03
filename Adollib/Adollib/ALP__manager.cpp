
#include "ALP__physics_function.h"
#include "ALP__manager.h"
#include "scene.h"
#include "Adollib.h"


using namespace Adollib;
using namespace Contacts;
using namespace physics_function;

#include "gameobject_manager.h"

#include "work_meter.h"

namespace Adollib
{

	std::unordered_map<Scenelist, std::vector<Adollib::Plane*>>  Rigitbody_manager::planes;
	std::unordered_map<Scenelist, std::vector<Adollib::Box*>> Rigitbody_manager::boxex;
	std::unordered_map<Scenelist, std::vector<Adollib::Sphere*>>Rigitbody_manager::spheres;
	std::unordered_map<Scenelist, std::vector<Adollib::Meshcoll*>>Rigitbody_manager::meshcolls;

	std::vector<Contacts::Contact_pair> Rigitbody_manager::pairs;

	bool Rigitbody_manager::update(Scenelist Sce)
	{
		std::vector<Contact> contacts;
		float resituation = Al_Global::base_resituation;
		int object_num = spheres[Sce].size() + boxex[Sce].size() + planes[Sce].size();
		if (object_num == 0)return false;

		std::vector<Collider*> colls;		//簡単にするために一つの配列にまとめる

		for (int i = 0; i < object_num; i++) {
			Collider* co;
			if (i < spheres[Sce].size()) {
				co = spheres[Sce][i];
			}
			else if (i < spheres[Sce].size() + boxex[Sce].size()) {
				co = boxex[Sce][i - spheres[Sce].size()];
			}
			else {
				co = planes[Sce][i - spheres[Sce].size() - boxex[Sce].size()];
			}

			co->gameobject->co_e.orient = quaternion_identity();
			co->gameobject->co_e.position = vector3(0, 0, 0);

			colls.emplace_back(co);
			//if (RB_s.rigit_body->is_movable() == false) no_move_obj.push_back(&RBs.back());
		}

		for (int i = 0; i < colls.size(); i++) {
			//world空間での情報を更新
			colls[i]->update_world_trans();
		}

		// 外力の更新
		applyexternalforce(colls);

		static std::vector<Contacts::Collider_2> broad_mid_pair;

		// 大雑把な当たり判定
		Work_meter::start("Broad,Mid,Narrow");
		Work_meter::start("Broadphase");
		Work_meter::tag_start("Broadphase");
		Broadphase(colls, broad_mid_pair, pairs);
		Work_meter::tag_stop();
		Work_meter::stop("Broadphase");

		Work_meter::start("Midphase");
		Work_meter::tag_start("Midphase");
		Midphase(broad_mid_pair, pairs);
		Work_meter::tag_stop();
		Work_meter::stop("Midphase");

		// 衝突生成
		Work_meter::start("Narrowphase");
		Work_meter::tag_start("Narrowphase");
		generate_contact(pairs);
		Work_meter::tag_stop();
		Work_meter::stop("Narrowphase");

		Work_meter::stop("Broad,Mid,Narrow");

		// 衝突解決
		Work_meter::start("Resolve");
		Work_meter::tag_start("Resolve");
		resolve_contact(colls, pairs);
		Work_meter::tag_stop();
		Work_meter::stop("Resolve");

		//位置の更新
		integrate(colls);

		for (int i = 0; i < colls.size(); i++) {
			//colliderの影響をアタッチされたgoへ
			colls[i]->resolve_gameobject();
		}

		return true;

	}


		void Rigitbody_manager::destroy(Scenelist Sce) {
			spheres[Sce].clear();
			boxex[Sce].clear();
			planes[Sce].clear();
			meshcolls[Sce].clear();
		}
	}


namespace Adollib {
	void Rigitbody_manager::add_collider(Adollib::Sphere* R, Gameobject* for_drawing, std::string tag, std::vector<std::string> No_hit_tag, Scenelist Sce) {
		R->tag = tag;
		R->No_hit_tag = No_hit_tag;
		R->gameobject = for_drawing;

		spheres[Sce].emplace_back(R);
	}
	void Rigitbody_manager::add_collider(Adollib::Box* R, Gameobject* for_drawing, std::string tag, std::vector<std::string> No_hit_tag, Scenelist Sce) {
		R->tag = tag;
		R->No_hit_tag = No_hit_tag;
		R->gameobject = for_drawing;

		boxex[Sce].emplace_back(R);
	}
	void Rigitbody_manager::add_collider(Adollib::Plane* R, Gameobject* for_drawing, std::string tag, std::vector<std::string> No_hit_tag, Scenelist Sce) {
		R->tag = tag;
		R->No_hit_tag = No_hit_tag;
		R->gameobject = for_drawing;

		planes[Sce].emplace_back(R);
	}
	void Rigitbody_manager::add_collider(Adollib::Meshcoll* R, Gameobject* for_drawing, std::string tag, std::vector<std::string> No_hit_tag, Scenelist Sce) {
		R->tag = tag;
		R->No_hit_tag = No_hit_tag;
		R->gameobject = for_drawing;

		meshcolls[Sce].emplace_back(R);
	}

	//void Rigitbody_manager::add_collider(Adollib::Sphere* R, Gameobject* for_drawing, std::string tag, std::vector<std::string> No_hit_tag, Scenelist Sce) {
	//	R->tag = tag;
	//	R->No_hit_tag = No_hit_tag;
	//	R->gameobject = for_drawing;

	//	spheres[Sce].emplace_back(R);
	//}
	//void Rigitbody_manager::add_collider(Adollib::Box* R, Gameobject* for_drawing, std::string tag, std::vector<std::string> No_hit_tag, Scenelist Sce) {
	//	R->tag = tag;
	//	R->No_hit_tag = No_hit_tag;
	//	R->gameobject = for_drawing;

	//	boxex[Sce].emplace_back(R);
	//}
	//void Rigitbody_manager::add_collider(Adollib::Plane* R, Gameobject* for_drawing, std::string tag, std::vector<std::string> No_hit_tag, Scenelist Sce) {
	//	R->tag = tag;
	//	R->No_hit_tag = No_hit_tag;
	//	R->gameobject = for_drawing;

	//	planes[Sce].emplace_back(R);
	//}
	//void Rigitbody_manager::add_collider(Adollib::Meshcoll* R, Gameobject* for_drawing, std::string tag, std::vector<std::string> No_hit_tag, Scenelist Sce) {
	//	R->tag = tag;
	//	R->No_hit_tag = No_hit_tag;
	//	R->gameobject = for_drawing;

	//	meshcolls[Sce].emplace_back(R);
	//}



}