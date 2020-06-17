

#include "rigit_body_manager.h"
#include "scene.h"
#include "Adollib.h"

using namespace Adollib;
using namespace Contacts;

namespace Adollib
{
	std::map<Scenelist, std::vector<Rigitbody_manager::RB_sphere>> Rigitbody_manager::RB_sphere_s;
	std::map<Scenelist, std::vector<Rigitbody_manager::RB_box>> Rigitbody_manager::RB_box_s;
	std::map<Scenelist, std::vector<Rigitbody_manager::RB_plane>> Rigitbody_manager::RB_plane_s;

	void Rigitbody_manager::update(Scenelist Sce)
	{
		std::vector<Contact> contacts;
		float resituation = Al_Global::resituation;
		int object_num = RB_sphere_s[Sce].size() + RB_box_s[Sce].size() + RB_plane_s[Sce].size();
		if (object_num == 0)return;

		//簡単にするために一つの配列にまとめる
		struct RB_struct {
			RB_base* RB;
			Rigitbody* rigit_body;
		};

		std::vector<RB_struct> RBs; //本体
		std::vector<RB_struct*> no_move_obj; //不動オブジェクトを保存(親みたいなもの)

		for (int i = 0; i < object_num; i++) {
			RB_struct RB_s;
			if (i < RB_sphere_s[Sce].size()) {
				RB_s.RB = &RB_sphere_s[Sce][i];
				RB_s.rigit_body = RB_sphere_s[Sce][i].R;
			}
			else if (i < RB_sphere_s[Sce].size() + RB_box_s[Sce].size()) {
				RB_s.RB = &RB_box_s[Sce][i - RB_sphere_s[Sce].size()];
				RB_s.rigit_body = RB_box_s[Sce][i - RB_sphere_s[Sce].size()].R;
			}
			else {
				RB_s.RB = &RB_plane_s[Sce][i - RB_sphere_s[Sce].size() - RB_box_s[Sce].size()];
				RB_s.rigit_body = RB_plane_s[Sce][i - RB_sphere_s[Sce].size() - RB_box_s[Sce].size()].R;
			}

			RB_s.RB->pearent->co_e.orient = quaternion_identity();
			RB_s.RB->pearent->co_e.position = vector3(0, 0, 0);

			RBs.push_back(RB_s);
			if (RB_s.rigit_body->is_movable() == false) no_move_obj.push_back(&RBs.back());
			
		}

#if USE_CHECK_CONTACT
#else
		for (int i = 0; i < RBs.size(); i++) {
			//world空間での情報を更新
			RBs[i].rigit_body->update_world_trans();

			//毎フレームの処理を行う
			RBs[i].rigit_body->integrate();
		}
#endif

		for (int i = 0; i < object_num; i++) {
			for (int o = 0; o < object_num; o++) {
				if (i <= o)continue;

				//衝突しない者同士は省く
				{
					bool no_hit = false;
					for (int n = 0; n < RBs[i].RB->No_hit_tag.size(); n++) {
						if (RBs[i].RB->No_hit_tag[n] == std::string("all")) no_hit = true;
						else if (RBs[i].RB->No_hit_tag[n] == RBs[o].RB->tag) no_hit = true;
					}
					if (no_hit == true)continue;
					for (int n = 0; n < RBs[o].RB->No_hit_tag.size(); n++) {
						if (RBs[o].RB->No_hit_tag[n] == std::string("all")) no_hit = true;
						else if (RBs[o].RB->No_hit_tag[n] == RBs[1].RB->tag) no_hit = true;
					}
					if (no_hit == true)continue;
				}

				switch (RBs[i].rigit_body->shape) {
				case Rigitbody_shape::shape_sphere:
					if (RBs[o].rigit_body->shape == Rigitbody_shape::shape_sphere) generate_contact_sphere_sphere(*RB_sphere_s[Sce][i].R, *RB_sphere_s[Sce][o].R, contacts, resituation);
					else if (RBs[o].rigit_body->shape == Rigitbody_shape::shape_box)    generate_contact_sphere_box(*RB_sphere_s[Sce][i].R, *RB_box_s[Sce][o - RB_sphere_s[Sce].size()].R, contacts, resituation);
					else if (RBs[o].rigit_body->shape == Rigitbody_shape::shape_plane)  generate_contact_sphere_plane(*RB_sphere_s[Sce][i].R, *RB_plane_s[Sce][o - RB_sphere_s[Sce].size() - RB_box_s[Sce].size()].R, contacts, resituation);
					break;

				case Rigitbody_shape::shape_box:
					if (RBs[o].rigit_body->shape == Rigitbody_shape::shape_sphere) generate_contact_sphere_box(*RB_sphere_s[Sce][o].R, *RB_box_s[Sce][i - RB_sphere_s[Sce].size()].R, contacts, resituation);
					else if (RBs[o].rigit_body->shape == Rigitbody_shape::shape_box)    generate_contact_box_box(*RB_box_s[Sce][i - RB_sphere_s[Sce].size()].R, *RB_box_s[Sce][o - RB_sphere_s[Sce].size()].R, contacts, resituation);
					else if (RBs[o].rigit_body->shape == Rigitbody_shape::shape_plane)  generate_contact_box_plane(*RB_box_s[Sce][i - RB_sphere_s[Sce].size()].R, *RB_plane_s[Sce][o - RB_sphere_s[Sce].size() - RB_box_s[Sce].size()].R, contacts, resituation);
					break;
				case Rigitbody_shape::shape_plane:
					if (RBs[o].rigit_body->shape == Rigitbody_shape::shape_sphere) generate_contact_sphere_plane(*RB_sphere_s[Sce][o].R, *RB_plane_s[Sce][i - RB_sphere_s[Sce].size() - RB_box_s[Sce].size()].R, contacts, resituation);
					else if (RBs[o].rigit_body->shape == Rigitbody_shape::shape_box)    generate_contact_box_plane(*RB_box_s[Sce][o - RB_sphere_s[Sce].size()].R, *RB_plane_s[Sce][i - RB_sphere_s[Sce].size() - RB_box_s[Sce].size()].R, contacts, resituation);
					//else if (RB_2->collider_type == Collidertype::Plane)  generate_contact_sphere_plane(*RB_sphere_s[i].R, *RB_plane_s[o - RB_sphere_s[Sce].size() - RB_box_s.size()].R, contacts, resituation);
					break;
				default:
					break;
				}

			}
		}

#if  0
		for (int i = 0; i < no_move_obj.size(); i++) {


		}


#elif 1
		//Contactのソート(下から順に更新するように)
		std::list<Contact> contact_sorted;
		for (int i = 0; i < contacts.size(); i++) {
			std::list<Contact>::iterator itr = contact_sorted.begin();
			std::list<Contact>::iterator itr_end = contact_sorted.end();

			for (;;) {
				if (itr == itr_end) {
					contact_sorted.push_back(contacts[i]);
					break;
				}
				if (itr->point.y > contacts[i].point.y) {
					contact_sorted.insert(itr, contacts[i]);
					break;
				}
				itr++;
			}

		}

		//ちゃんとソートできているかの確認
		assert(contact_sorted.size() == contacts.size());

		{
			std::list<Contact>::iterator itr = contact_sorted.begin();
			std::list<Contact>::iterator itr_end = contact_sorted.end();
			for (; itr != itr_end; itr++) {
				itr->resolve();
			}
		}
#else

		for (int i = 0; i < contacts.size(); i++) {
			//衝突の解決
			contacts[i].resolve();
		}
#endif

		for (int i = 0; i < RBs.size(); i++) {
			//colliderの影響をアタッチされたgoへ
			RBs[i].rigit_body->resolve_gameobject();
		}

#if USE_CHECK_CONTACT
		for (int i = 0; i < rigit_bodys.size(); i++) {
			rigit_bodys[i]->integrate();
		}
#endif

	}

	void Rigitbody_manager::destroy(Scenelist Sce) {
		RB_sphere_s[Sce].clear();
		RB_box_s[Sce].clear();
		RB_plane_s[Sce].clear();
	}
}

namespace Adollib {
	void Rigitbody_manager::set_rigitbody(Adollib::Sphere* R, Gameobject* for_drawing, std::string tag, std::vector<std::string> No_hit_tag, Scenelist Sce) {
		RB_sphere push_A;
		push_A.R = R;
		push_A.tag = tag;
		push_A.No_hit_tag = No_hit_tag;
		push_A.pearent = for_drawing;

		RB_sphere_s[Sce].push_back(push_A);
	}
	void Rigitbody_manager::set_rigitbody(Adollib::Box* R, Gameobject* for_drawing, std::string tag, std::vector<std::string> No_hit_tag, Scenelist Sce) {
		RB_box push_A;
		push_A.R = R;
		push_A.tag = tag;
		push_A.No_hit_tag = No_hit_tag;
		push_A.pearent = for_drawing;

		RB_box_s[Sce].push_back(push_A);
	}
	void Rigitbody_manager::set_rigitbody(Adollib::Plane* R, Gameobject* for_drawing, std::string tag, std::vector<std::string> No_hit_tag, Scenelist Sce) {
		RB_plane push_A;
		push_A.R = R;
		push_A.tag = tag;
		push_A.No_hit_tag = No_hit_tag;
		push_A.pearent = for_drawing;

		RB_plane_s[Sce].push_back(push_A);
	}

}