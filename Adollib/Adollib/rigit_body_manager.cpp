

#include "rigit_body_manager.h"
#include "scene.h"

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
		float resituation = 0.4;
		int object_num = RB_sphere_s[Sce].size() + RB_box_s[Sce].size() + RB_plane_s[Sce].size();
		if (object_num == 0)return;

		//簡単にするために一つの配列にまとめる
		struct RB_struct {
			RB_base* RB;
			Rigitbody* rigit_body;
		};

		std::vector<RB_struct> RBs;

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
		}

#if USE_CHECK_CONTACT
#else
		for (int i = 0; i < RBs.size(); i++) {
			RBs[i].rigit_body->update_world_trans();
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

				switch (RBs[i].RB->collider_type) {
				case Collidertype::Sphere:
					if (RBs[o].RB->collider_type == Collidertype::Sphere) generate_contact_sphere_sphere(*RB_sphere_s[Sce][i].R, *RB_sphere_s[Sce][o].R, contacts, resituation);
					else if (RBs[o].RB->collider_type == Collidertype::Box)    generate_contact_sphere_box(*RB_sphere_s[Sce][i].R, *RB_box_s[Sce][o - RB_sphere_s[Sce].size()].R, contacts, resituation);
					else if (RBs[o].RB->collider_type == Collidertype::Plane)  generate_contact_sphere_plane(*RB_sphere_s[Sce][i].R, *RB_plane_s[Sce][o - RB_sphere_s[Sce].size() - RB_box_s[Sce].size()].R, contacts, resituation);
					break;

				case Collidertype::Box:
					if (RBs[o].RB->collider_type == Collidertype::Sphere) generate_contact_sphere_box(*RB_sphere_s[Sce][o].R, *RB_box_s[Sce][i - RB_sphere_s[Sce].size()].R, contacts, resituation);
					else if (RBs[o].RB->collider_type == Collidertype::Box)    generate_contact_box_box(*RB_box_s[Sce][i - RB_sphere_s[Sce].size()].R, *RB_box_s[Sce][o - RB_sphere_s[Sce].size()].R, contacts, resituation);
					else if (RBs[o].RB->collider_type == Collidertype::Plane)  generate_contact_box_plane(*RB_box_s[Sce][i - RB_sphere_s[Sce].size()].R, *RB_plane_s[Sce][o - RB_sphere_s[Sce].size() - RB_box_s[Sce].size()].R, contacts, resituation);
					break;
				case Collidertype::Plane:
					if (RBs[o].RB->collider_type == Collidertype::Sphere) generate_contact_sphere_plane(*RB_sphere_s[Sce][o].R, *RB_plane_s[Sce][i - RB_sphere_s[Sce].size() - RB_box_s[Sce].size()].R, contacts, resituation);
					else if (RBs[o].RB->collider_type == Collidertype::Box)    generate_contact_box_plane(*RB_box_s[Sce][o - RB_sphere_s[Sce].size()].R, *RB_plane_s[Sce][i - RB_sphere_s[Sce].size() - RB_box_s[Sce].size()].R, contacts, resituation);
					//else if (RB_2->collider_type == Collidertype::Plane)  generate_contact_sphere_plane(*RB_sphere_s[i].R, *RB_plane_s[o - RB_sphere_s[Sce].size() - RB_box_s.size()].R, contacts, resituation);
					break;
				default:
					break;
				}

			}
		}


		for (int i = 0; i < contacts.size(); i++) {
			//衝突の解決
			contacts[i].resolve();
		}
		for (int i = 0; i < RBs.size(); i++) {
			//colliderの影響をアタッチされたgoへ
			RBs[i].rigit_body->resolve_gameobject();
		}


#if USE_CHECK_CONTACT
		for (int i = 0; i < rigit_bodys.size(); i++) {
			rigit_bodys[i]->integrate();
		}
#endif

		//int nan_num = 0;
		//for (int i = 0; i < RB_sphere_s[Sce].size(); i++) {
		//	RB_sphere_s[i].for_drawing->transform->local_position = RB_sphere_s[i].R->position.get_XM3();

		//	if (isnan(RB_sphere_s[i].R->position.norm() + RB_sphere_s[i].R->orientation.norm())) {
		//		nan_num++;
		//	}

		//}
		//for (int i = 0; i < RB_box_s.size(); i++) {
		//	RB_box_s[i].for_drawing->transform->local_position = RB_box_s[i].R->position.get_XM3();
		//	RB_box_s[i].for_drawing->transform->local_rotation = RB_box_s[i].R->orientation.euler().get_XM3();
		//	RB_box_s[i].for_drawing->transform->local_scale = RB_box_s[i].R->half_size.get_XM3();

		//	if (isnan(RB_box_s[i].R->position.norm() + RB_box_s[i].R->orientation.norm())) {
		//		nan_num++;
		//	}

		//}
		//for (int i = 0; i < RB_plane_s[Sce].size(); i++) {
		//	RB_plane_s[i].for_drawing->transform->local_position = RB_plane_s[i].R->position.get_XM3();

		//	if (isnan(RB_plane_s[i].R->angular_velocity.norm())) {
		//		nan_num++;
		//	}
		//}

		//framework::debug->setString("nan_nam : %d", nan_num);


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

		push_A.collider_type = Collidertype::Sphere;

		RB_sphere_s[Sce].push_back(push_A);
	}
	void Rigitbody_manager::set_rigitbody(Adollib::Box* R, Gameobject* for_drawing, std::string tag, std::vector<std::string> No_hit_tag, Scenelist Sce) {
		RB_box push_A;
		push_A.R = R;
		push_A.tag = tag;
		push_A.No_hit_tag = No_hit_tag;
		push_A.pearent = for_drawing;

		push_A.collider_type = Collidertype::Box;

		RB_box_s[Sce].push_back(push_A);
	}
	void Rigitbody_manager::set_rigitbody(Adollib::Plane* R, Gameobject* for_drawing, std::string tag, std::vector<std::string> No_hit_tag, Scenelist Sce) {
		RB_plane push_A;
		push_A.R = R;
		push_A.tag = tag;
		push_A.No_hit_tag = No_hit_tag;
		push_A.pearent = for_drawing;

		push_A.collider_type = Collidertype::Plane;

		RB_plane_s[Sce].push_back(push_A);
	}

}