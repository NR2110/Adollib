

#include "rigit_body_manager.h"


using namespace Adollib;
using namespace Contacts;

namespace Adollib
{
	// 所属するシーンの初期化時に一度だけ呼ばれる
	void Rigitbody_manager::awake() {

	}

	void Rigitbody_manager::start()
	{

	}

	// 毎フレーム呼ばれる更新処理
	void Rigitbody_manager::update()
	{

		std::vector<Contact> contacts;
		float resituation = 0.4;
		int object_num = RB_sphere_s.size() + RB_box_s.size() + RB_plane_s.size();

		//簡単にするために一つの配列にまとめる
		std::vector<RB_base*> RBs;
		std::vector<Rigitbody*> rigit_bodys;

		for (int i = 0; i < object_num; i++) {
			if (i < RB_sphere_s.size()) RBs.push_back(&RB_sphere_s[i]);
			else if (i < RB_sphere_s.size() + RB_box_s.size()) RBs.push_back(&RB_box_s[i - RB_sphere_s.size()]);
			else RBs.push_back(&RB_plane_s[i - RB_sphere_s.size() - RB_box_s.size()]);

			if (i < RB_sphere_s.size()) rigit_bodys.push_back(RB_sphere_s[i].R);
			else if (i < RB_sphere_s.size() + RB_box_s.size()) rigit_bodys.push_back(RB_box_s[i - RB_sphere_s.size()].R);
			else rigit_bodys.push_back(RB_plane_s[i - RB_sphere_s.size() - RB_box_s.size()].R);
		}

#if USE_CHECK_CONTACT
#else
		for (int i = 0; i < rigit_bodys.size(); i++) {
			rigit_bodys[i]->integrate();
		}
#endif

		for (int i = 0; i < object_num; i++) {
			for (int o = 0; o < object_num; o++) {
				if (i <= o)continue;

				//衝突しない者同士は省く
				{
					bool no_hit = false;
					for (int n = 0; n < RBs[i]->No_hit_tag.size(); n++) {
						if (RBs[i]->No_hit_tag[n] == RBs[o]->tag) no_hit = true;
					}
					if (no_hit == true)continue;
					for (int n = 0; n < RBs[o]->No_hit_tag.size(); n++) {
						if (RBs[o]->No_hit_tag[n] == RBs[1]->tag) no_hit = true;
					}
					if (no_hit == true)continue;
				}

				switch (RBs[i]->collider_type) {
				case Collidertype::Sphere:
					if (RBs[o]->collider_type == Collidertype::Sphere) generate_contact_sphere_sphere(*RB_sphere_s[i].R, *RB_sphere_s[o].R, contacts, resituation);
					else if (RBs[o]->collider_type == Collidertype::Box)    generate_contact_sphere_box(*RB_sphere_s[i].R, *RB_box_s[o - RB_sphere_s.size()].R, contacts, resituation);
					else if (RBs[o]->collider_type == Collidertype::Plane)  generate_contact_sphere_plane(*RB_sphere_s[i].R, *RB_plane_s[o - RB_sphere_s.size() - RB_box_s.size()].R, contacts, resituation);
					break;

				case Collidertype::Box:
					if (RBs[o]->collider_type == Collidertype::Sphere) generate_contact_sphere_box(*RB_sphere_s[o].R, *RB_box_s[i - RB_sphere_s.size()].R, contacts, resituation);
					else if (RBs[o]->collider_type == Collidertype::Box)    generate_contact_box_box(*RB_box_s[i - RB_sphere_s.size()].R, *RB_box_s[o - RB_sphere_s.size()].R, contacts, resituation);
					else if (RBs[o]->collider_type == Collidertype::Plane)  generate_contact_box_plane(*RB_box_s[i - RB_sphere_s.size()].R, *RB_plane_s[o - RB_sphere_s.size() - RB_box_s.size()].R, contacts, resituation);
					break;
				case Collidertype::Plane:
					if (RBs[o]->collider_type == Collidertype::Sphere) generate_contact_sphere_plane(*RB_sphere_s[o].R, *RB_plane_s[i - RB_sphere_s.size() - RB_box_s.size()].R, contacts, resituation);
					else if (RBs[o]->collider_type == Collidertype::Box)    generate_contact_box_plane(*RB_box_s[o - RB_sphere_s.size()].R, *RB_plane_s[i - RB_sphere_s.size() - RB_box_s.size()].R, contacts, resituation);
					//else if (RB_2->collider_type == Collidertype::Plane)  generate_contact_sphere_plane(*RB_sphere_s[i].R, *RB_plane_s[o - RB_sphere_s.size() - RB_box_s.size()].R, contacts, resituation);
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

#if USE_CHECK_CONTACT
		for (int i = 0; i < rigit_bodys.size(); i++) {
			rigit_bodys[i]->integrate();
		}
#endif

		//int nan_num = 0;
		//for (int i = 0; i < RB_sphere_s.size(); i++) {
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
		//for (int i = 0; i < RB_plane_s.size(); i++) {
		//	RB_plane_s[i].for_drawing->transform->local_position = RB_plane_s[i].R->position.get_XM3();

		//	if (isnan(RB_plane_s[i].R->angular_velocity.norm())) {
		//		nan_num++;
		//	}
		//}

		//framework::debug->setString("nan_nam : %d", nan_num);


	}
	// 毎フレーム、update()後に呼ばれる更新処理
	void Rigitbody_manager::lateUpdate()
	{


	}

	// このスクリプトがアタッチされているGOのactiveSelfがtrueになった時呼ばれる
	void Rigitbody_manager::onEnable()
	{

	}

	// このスクリプトがアタッチされているGOのactiveSelfがfalseになった時呼ばれる
	void Rigitbody_manager::onDisable()
	{

	}

}

namespace Adollib {
	void Rigitbody_manager::set_rigitbody(Adollib::Sphere* R, Gameobject* for_drawing, std::string tag, std::vector<std::string> No_hit_tag) {
		RB_sphere push_A;
		push_A.R = R;
		push_A.tag = tag;
		push_A.No_hit_tag = No_hit_tag;
		push_A.for_drawing = for_drawing;

		push_A.collider_type = Collidertype::Sphere;

		RB_sphere_s.push_back(push_A);
	}
	void Rigitbody_manager::set_rigitbody(Adollib::Box* R, Gameobject* for_drawing, std::string tag, std::vector<std::string> No_hit_tag) {
		RB_box push_A;
		push_A.R = R;
		push_A.tag = tag;
		push_A.No_hit_tag = No_hit_tag;
		push_A.for_drawing = for_drawing;

		push_A.collider_type = Collidertype::Box;

		RB_box_s.push_back(push_A);
	}
	void Rigitbody_manager::set_rigitbody(Adollib::Plane* R, Gameobject* for_drawing, std::string tag, std::vector<std::string> No_hit_tag) {
		RB_plane push_A;
		push_A.R = R;
		push_A.tag = tag;
		push_A.No_hit_tag = No_hit_tag;
		push_A.for_drawing = for_drawing;

		push_A.collider_type = Collidertype::Plane;

		RB_plane_s.push_back(push_A);
	}

}