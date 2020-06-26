

#include "rigit_body_manager.h"
#include "scene.h"
#include "Adollib.h"

#include "physics_function.h"

using namespace Adollib;
using namespace Contacts;
using namespace physics_function;

#include "gameobject_manager.h"

namespace Adollib
{
	std::map<Scenelist, std::vector<Adollib::Plane*>>  Rigitbody_manager::RB_plane_s;
	std::map<Scenelist, std::vector<Adollib::Box*>> Rigitbody_manager::RB_box_s;
	std::map<Scenelist, std::vector<Adollib::Sphere*>>Rigitbody_manager::RB_sphere_s;

	std::vector<Contacts::Contact_pair> Rigitbody_manager::pairs;

	void Rigitbody_manager::update(Scenelist Sce)
	{
		static bool init = true;
		static Gameobject* S[4][2];
		if (init) {
			init = false;
			for (int i = 0; i < 4; i++) {
				S[i][0] = Gameobject_manager::createSphere();
				S[i][0]->transform->local_scale = vector3(0.2);
				S[i][0]->material->color = vector4(0, 0, 1, 1);
				S[i][0]->material->RS_state = State_manager::RStypes::RS_CULL_NONE;

				S[i][1] = Gameobject_manager::createSphere();
				S[i][1]->transform->local_scale = vector3(0.2);
				S[i][1]->material->color = vector4(1, 0, 0, 1);
				S[i][1]->material->RS_state = State_manager::RStypes::RS_CULL_NONE;
			}
		}

		std::vector<Contact> contacts;
		float resituation = Al_Global::resituation;
		int object_num = RB_sphere_s[Sce].size() + RB_box_s[Sce].size() + RB_plane_s[Sce].size();
		if (object_num == 0)return;

		std::vector<Collider*> colls;		//簡単にするために一つの配列にまとめる

		for (int i = 0; i < object_num; i++) {
			Collider* co;
			if (i < RB_sphere_s[Sce].size()) {
				co = RB_sphere_s[Sce][i];
			}
			else if (i < RB_sphere_s[Sce].size() + RB_box_s[Sce].size()) {
				co = RB_box_s[Sce][i - RB_sphere_s[Sce].size()];
			}
			else {
				co = RB_plane_s[Sce][i - RB_sphere_s[Sce].size() - RB_box_s[Sce].size()];
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

		// 大雑把な当たり判定
		Boardphase(colls, pairs);

		// 衝突生成
		generate_contact(pairs);

		// 衝突解決
		resolve_contact(colls, pairs);

		// 位置の更新
		integrate(colls);

		for (int i = 0; i < 4; i++) {
			S[i][0]->transform->local_pos = vector3(100000, 100000, 100000);
			S[i][1]->transform->local_pos = vector3(100000, 100000, 100000);
		}
		for (int i = 0; pairs.size() > 0 && i < pairs[0].contacts.contact_num; i++) {
			S[i][0]->transform->local_pos = vector3_be_rotated_by_quaternion(pairs[0].contacts.contactpoints[i].point[0], pairs[0].body[0]->world_orientation) + pairs[0].body[0]->world_position;

			S[i][1]->transform->local_pos = vector3_be_rotated_by_quaternion(pairs[0].contacts.contactpoints[i].point[1], pairs[0].body[1]->world_orientation) + pairs[0].body[1]->world_position;
		}


		for (int i = 0; i < colls.size(); i++) {
			//colliderの影響をアタッチされたgoへ
			colls[i]->resolve_gameobject();
		}
	}


		void Rigitbody_manager::destroy(Scenelist Sce) {
			RB_sphere_s[Sce].clear();
			RB_box_s[Sce].clear();
			RB_plane_s[Sce].clear();
		}
	}


namespace Adollib {
	void Rigitbody_manager::set_rigitbody(Adollib::Sphere* R, Gameobject* for_drawing, std::string tag, std::vector<std::string> No_hit_tag, Scenelist Sce) {
		R->tag = tag;
		R->No_hit_tag = No_hit_tag;
		R->gameobject = for_drawing;

		RB_sphere_s[Sce].emplace_back(R);
	}
	void Rigitbody_manager::set_rigitbody(Adollib::Box* R, Gameobject* for_drawing, std::string tag, std::vector<std::string> No_hit_tag, Scenelist Sce) {
		R->tag = tag;
		R->No_hit_tag = No_hit_tag;
		R->gameobject = for_drawing;

		RB_box_s[Sce].emplace_back(R);
	}
	void Rigitbody_manager::set_rigitbody(Adollib::Plane* R, Gameobject* for_drawing, std::string tag, std::vector<std::string> No_hit_tag, Scenelist Sce) {
		R->tag = tag;
		R->No_hit_tag = No_hit_tag;
		R->gameobject = for_drawing;

		RB_plane_s[Sce].emplace_back(R);
	}

}