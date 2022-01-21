
#include "title_Stage.h"

#include "../Adollib/Scripts/Main/systems.h"
#include "../Adollib/Scripts/Main/Adollib.h"
#include "../Adollib/Scripts/Imgui/debug.h"
#include "../Adollib/Scripts/Object/transform.h"
#include "../Adollib/Scripts/Object/gameobject.h"
#include "../Adollib/Scripts/Object/gameobject_manager.h"
#include "../Adollib/Scripts/Renderer/renderer_base.h"
#include "../Adollib/Scripts/Renderer/material_manager.h"
#include "../Adollib/Scripts/Renderer/texture.h"
#include "../Adollib/Scripts/Renderer/croth_renderer.h"
#include "../Adollib/Scripts/Physics/collider_croth.h"
#include "../Adollib/Scripts/Physics/collider_rope.h"
#include "../Adollib/Scripts/Physics/joint.h"

#include "../Adollib/Imgui/imgui.h"

#include "../Adollib/Scripts/Physics/ALP__physics_manager.h"

using namespace Adollib;

Collider* Title_stage::set_box(const char* name, Vector3 position, Vector3 rotate, Vector3 scale, Vector4 color, Gameobject* pearent, bool is_moveable) {
	auto go = Gameobject_manager::createCube(name);
	go->transform->local_scale = scale;
	go->transform->local_orient = quaternion_from_euler(rotate);
	go->transform->local_pos = position;
	go->renderer->color = color;
	if (pearent != nullptr)pearent->add_child(go);


	auto coll = go->addComponent<Collider>();
	coll->add_shape<Box>();
	if (is_moveable == true) {
		coll->tag = Collider_tags::Kinematic_Stage;
		coll->physics_data.inertial_mass = scale.x * scale.y * scale.z;
	}
	else {
		coll->tag = Collider_tags::Static_Stage;
		coll->physics_data.is_moveable = false;
	}

	return coll;
}

Collider* Title_stage::set_gear(const char* name, Vector3 position, Vector3 rotate, Vector3 scale, Vector4 color, int gear_tooth_size, Gameobject* pearent, bool is_moveable) {

		const Vector3 size = scale;

		Gameobject* GEAR_p = Gameobject_manager::create("GEAR");
		Gameobject* GEAR = Gameobject_manager::create("tooths");
		GEAR->transform->local_pos = position;
		GEAR->transform->local_orient = quaternion_from_euler(rotate);

		Collider* coll = GEAR->addComponent<Collider>();
		coll->tag |= Collider_tags::Kinematic_Stage;
		coll->physics_data.inertial_mass = scale.x * scale.y * scale.z * gear_tooth_size;
		coll->physics_data.is_kinmatic_linear = false;
		coll->physics_data.angula_sleep_threrhold = 0;
		coll->physics_data.is_fallable = false;
		coll->physics_data.anglar_drag = 0;
		coll->physics_data.drag = 1;
		coll->physics_data.restitution = 1;
		coll->physics_data.dynamic_friction = 0;

		//auto tensor = Matrix33(100000, 100000, 100000, 100000, 100000, 100000, 0, 0, coll->physics_data.inertial_mass);
		//coll->set_tensor(tensor);
		//coll->set_tensor(make_box_tensor(Vector3(1, 1000, 1), coll->physics_data.inertial_mass));

		for (int gear_tooth_num = 0; gear_tooth_num < gear_tooth_size; gear_tooth_num++) {
			Gameobject* GO = Gameobject_manager::createCube("tooth");
			GO->transform->local_scale = size;
			GO->transform->local_orient = quaternion_from_euler(0, 0, 360.0f / gear_tooth_size * gear_tooth_num);
			GO->transform->local_pos = Vector3(0, size.y + 3, 0);
			GO->transform->local_pos = vector3_quatrotate(GO->transform->local_pos, quaternion_axis_angle(Vector3(0, 0, 1), 360.0f / gear_tooth_size * gear_tooth_num));

			GO->renderer->color = color;
			GEAR->add_child(GO);
		}

		for (int gear_tooth_num = 0; gear_tooth_num < gear_tooth_size; gear_tooth_num++) {
			Box* box = coll->add_shape<Box>();
			box->size = size;
			box->rotate = Vector3(0, 0, 360.0f / gear_tooth_size * gear_tooth_num);
			box->center = Vector3(0, size.y + 3, 0);
			box->center = vector3_quatrotate(box->center, quaternion_axis_angle(Vector3(0, 0, 1), 360.0f / gear_tooth_size * gear_tooth_num));

		}

		Gameobject* gear_joint = Gameobject_manager::createSphere("gear_joint");

		gear_joint->transform->local_pos = position;
		gear_joint->transform->local_orient = quaternion_from_euler(rotate);
		Collider* gear_joint_collider = gear_joint->addComponent<Collider>();
		gear_joint_collider->physics_data.is_moveable = false;

		Joint::add_Hingejoint(gear_joint_collider, coll, Vector3(0, 0, +10), Vector3(0, 0, -10), Vector3(0, 0, +10), Vector3(0, 0, -10), 0.3f, 1000);

		GEAR_p->add_child(GEAR);
		GEAR_p->add_child(gear_joint);
		if (pearent != nullptr) {
			pearent->add_child(GEAR_p);
		}
}

// 所属するシーンの初期化時に一度だけ呼ばれる
void Title_stage::awake()
{
	wall_color = Vector4(234, 234, 234, 255) / 255.0f;
	//floar_color = Vector4(57, 34, 34, 255) / 255.0f;
	floar_color = Vector4(179, 179, 179, 255) / 255.0f;
	stair_color = Vector4(64, 64, 64, 255) / 255.0f;

	Collider* wall_coll = nullptr;
	{
		wall_coll = set_box("Wall", Vector3(0, 0, 1), Vector3(0), Vector3(100, 100, 1), wall_color);
		set_box("Floar", Vector3(0, -33, -3), Vector3(-1, 0, 0), Vector3(100, 1, 3), floar_color);
	}
	{
		auto title = Gameobject_manager::createPlane("Title_rogo");
		title->transform->local_pos = Vector3(0, 3.6f, -0.1f);
		title->transform->local_orient = quaternion_from_euler(0, 180, 0);
		title->transform->local_scale = Vector3(75, 35, 0);
		auto title_material = Material_manager::create_material("Title_rogo");
		title_material->get_texture()->Load(L"./DefaultTexture/title/title_rogo.png");
		title_material->BS_state = State_manager::BStypes::BS_ALPHA;
		title_material->Load_VS("./DefaultShader/sprite_vs.cso");
		title_material->Load_PS("./DefaultShader/sprite_ps.cso");
		title->renderer->set_material(title_material);
	}

	{
		auto title = Gameobject_manager::create("Title_pushstart");
		title->transform->local_pos = Vector3(-1, 50, -9);
		title->transform->local_orient = quaternion_from_euler(0, 180, 0);
		title->transform->local_scale = Vector3(1, 1, 0.1f);
		auto title_sprite = Gameobject_manager::createPlane("Title_pushstart");
		title->add_child(title_sprite);

		//title_sprite->transform->local_pos = Vector3(-1, 100, -7);
		title_sprite->transform->local_scale = Vector3(31, 13, 0.5f);
		auto title_material = Material_manager::create_material("Title_pushstart");
		title_material->get_texture()->Load(L"./DefaultTexture/title/title_pushstart.png");
		title_material->BS_state = State_manager::BStypes::BS_ALPHA;
		title_material->RS_state = State_manager::RStypes::RS_CULL_NONE;
		title_material->Load_VS("./DefaultShader/sprite_vs.cso");
		title_material->Load_PS("./DefaultShader/sprite_ps.cso");
		title_sprite->renderer->set_material(title_material);

		auto coll = title->addComponent<Collider>();
		// spriteに合わせたcollider設定
		{
			{
				auto shape = coll->add_shape<Box>();
				shape->center = Vector3(27.7f, 0.3f, 0);
				shape->rotate = Vector3(0, 0, -5.2f);
				shape->size = Vector3(1, 1.5f, 1);
			}
			{
				auto shape = coll->add_shape<Box>();
				shape->center = Vector3(25.6f, -1.1f, 0);
				shape->rotate = Vector3(0, 0, -4.2f);
				shape->size = Vector3(1.9f, 3.1f, 1);
			}
			{
				auto shape = coll->add_shape<Box>();
				shape->center = Vector3(0.26f, 0, 0);
				shape->rotate = Vector3(0, 0, 0);
				shape->size = Vector3(24, 3.94f, 1);
			}
			{
				auto shape = coll->add_shape<Box>();
				shape->center = Vector3(0.26f, 2, -10);
				shape->rotate = Vector3(0, 0, 0);
				shape->size = Vector3(24, 1, 10);
			}
			{
				auto shape = coll->add_shape<Box>();
				shape->center = Vector3(12.5f, -3.95f, 0);
				shape->rotate = Vector3(0, 0, -8.20f);
				shape->size = Vector3(11, 1.73f, 1);
			}
			{
				auto shape = coll->add_shape<Box>();
				shape->center = Vector3(-10.72f, -3, 0);
				shape->rotate = Vector3(0, 0, -0.8f);
				shape->size = Vector3(14.4f, 1.5f, 1);
			}
			{
				auto shape = coll->add_shape<Box>();
				shape->center = Vector3(16.5f, 3.17f, 0);
				shape->rotate = Vector3(0, 0, 1.8f);
				shape->size = Vector3(7.78f, 1, 1);
			}
			{
				auto shape = coll->add_shape<Box>();
				shape->center = Vector3(-1.54f, 2.77f, 0);
				shape->rotate = Vector3(0, 0, -4.7f);
				shape->size = Vector3(10, 2, 1);
			}
			{
				auto shape = coll->add_shape<Box>();
				shape->center = Vector3(-17.35f, 3.9f, 0);
				shape->rotate = Vector3(0, 0, -2.6f);
				shape->size = Vector3(6.27f, 2, 1);
			}
			{
				auto shape = coll->add_shape<Box>();
				shape->center = Vector3(-25, 0.55f, 0);
				shape->rotate = Vector3(0, 0, -2.8f);
				shape->size = Vector3(1.2f, 1.5f, 1);
			}
		}

		coll->add_shape<Box>();
		coll->add_shape<Box>();
		coll->add_shape<Box>();
		coll->add_shape<Box>();
		coll->add_shape<Box>();
		coll->tag = Collider_tags::Title;
		coll->physics_data.inertial_mass = 300;
		coll->physics_data.anglar_drag = 0.5f;
		coll->physics_data.angula_sleep_threrhold = 0;
		//coll->physics_data.is_moveable = false;

		auto joint_a = Joint::add_balljoint(wall_coll, coll, Vector3(+30, 18, -7), Vector3(-30, 5, 0));
		auto joint_b = Joint::add_balljoint(wall_coll, coll, Vector3(-30, 18, -7), Vector3(+30, 5, 0));
		joint_a->offset = 14.5f + 18;
		joint_b->offset = 14.5f + 18;
		joint_a->stretch_bias = 0;
		joint_b->stretch_bias = 0;
	}

	{
		Gameobject* stair_pearent = Gameobject_manager::create("stair_pearent");
		set_box("stair_part", Vector3(-78, -18, -3), Vector3(-1, 0, -1), Vector3(15, 1, 3), stair_color, stair_pearent);
		set_box("stair_part", Vector3(-58, -20, -3), Vector3(1, 0,  -1), Vector3(3, 1, 3), stair_color, stair_pearent);
		set_box("stair_part", Vector3(-50, -22, -3), Vector3(1, 0,  -1), Vector3(3, 1, 3), stair_color, stair_pearent);
		set_box("stair_part", Vector3(-42, -24, -3), Vector3(1, 0,  -1), Vector3(3, 1, 3), stair_color, stair_pearent);
		set_box("stair_part", Vector3(-34, -26, -3), Vector3(1, 0,  -1), Vector3(3, 1, 3), stair_color, stair_pearent);
		set_box("stair_part", Vector3(-26, -28, -3), Vector3(1, 0,  -1), Vector3(3, 1, 3), stair_color, stair_pearent);
		set_box("stair_part", Vector3(-18, -30, -3), Vector3(1, 0,  -1), Vector3(3, 1, 3), stair_color, stair_pearent);
	}

	{
		Gameobject* cloth_pearent = Gameobject_manager::create("cloth_pearent");
		set_box("cloth_pillar", Vector3(-20, 0, -4), Vector3(-35,0,0), Vector3(1, 1, 21), Vector4(195, 195, 195, 255) / 255.0f, cloth_pearent);
		set_box("cloth_pillar", Vector3(+20, 0, -4), Vector3(-35,0,0), Vector3(1, 1, 21), Vector4(195, 195, 195, 255) / 255.0f, cloth_pearent);
		{
			auto croth = Gameobject_manager::createFromFBX("croth", "./DefaultModel/plane_16x16vertex.fbx");
			//croth->transform->local_scale = Vector3(20, 4, 10);
			croth->transform->local_scale = Vector3(20, 20, 20);
			croth->transform->local_pos = Vector3(0, 0, -5);
			croth->transform->local_orient = quaternion_from_euler(Vector3(55, 0, 0));
			//croth->transform->local_orient = quaternion_from_euler(Vector3(90, 0, 0));

			auto coll = croth->addComponent<Collider_Croth>();
			coll->default_physics_data.angula_sleep_threrhold = 0;
			coll->default_physics_data.linear_sleep_threrhold = 0;
			coll->ignore_tags = Collider_tags::Static_Stage;
			coll->load_file("./DefaultModel/plane_16x16vertex.fbx", true, false);
			croth->transform->local_scale = Vector3(19, 20, 20);
			{
				Physics_data data;

				data = coll->get_vertex_data(0, 225);
				data.is_moveable = false;
				coll->set_vertex_data(0, 225, data);
				data = coll->get_vertex_data(0, 227);
				data.is_moveable = false;
				coll->set_vertex_data(0, 227, data);
				for (int i = 274; i < 289; ++i) {
					data = coll->get_vertex_data(0, i);
					data.is_moveable = false;
					coll->set_vertex_data(0, i, data);
				}
				for (int i = 242; i < 260; ++i) {
					if (i == 258)continue;
					data = coll->get_vertex_data(0, i);
					data.is_moveable = false;
					coll->set_vertex_data(0, i, data);
				}
			}

			auto renderer = croth->addComponent<Croth_renderer>();
			renderer->set_meshoffset(coll->get_vertex_offset());
			renderer->color = Vector4(159, 159, 255, 255) / 255.0f;

			cloth_pearent->add_child(croth);
		}

			//cloth_pearent->transform->local_pos = Vector3(58, 18, 0);
			cloth_pearent->transform->local_pos = Vector3(58, 20, 0);
			//cloth_pearent->transform->local_orient = quaternion_from_euler(Vector3(18.2f, 2.2f, 51));
			cloth_pearent->transform->local_orient = quaternion_from_euler(Vector3(0, 0, 44));
	}
	{
		Vector4 tana_color = Vector4(254, 159, 159, 255) / 255.0f;
		Gameobject* tana_pearent = Gameobject_manager::create("tana_pearent");
		set_box("tana", Vector3(55, -28, -2), Vector3(0), Vector3(30, 1, 2), stair_color, tana_pearent);
		set_box("tana", Vector3(82, -30, -1), Vector3(0), Vector3(0.50f, 2, 0.50f), stair_color, tana_pearent);
		set_box("tana", Vector3(28, -30, -1), Vector3(0), Vector3(0.75f, 2, 0.75f), stair_color, tana_pearent);
		set_box("moveable_tana", Vector3(37, -26, -2), Vector3(0), Vector3(5, 1, 1.5f), tana_color, tana_pearent,true);
		set_box("moveable_tana", Vector3(49, -26, -2), Vector3(0), Vector3(5, 1, 1.5f), tana_color, tana_pearent,true);
		set_box("moveable_tana", Vector3(61, -26, -2), Vector3(0), Vector3(5, 1, 1.5f), tana_color, tana_pearent,true);
		set_box("moveable_tana", Vector3(73, -26, -2), Vector3(0), Vector3(5, 1, 1.5f), tana_color, tana_pearent,true);

		set_box("moveable_tana", Vector3(43, -24, -2), Vector3(0), Vector3(5, 1, 1.3f), tana_color, tana_pearent,true);
		set_box("moveable_tana", Vector3(55, -24, -2), Vector3(0), Vector3(5, 1, 1.3f), tana_color, tana_pearent,true);
		set_box("moveable_tana", Vector3(67, -24, -2), Vector3(0), Vector3(5, 1, 1.3f), tana_color, tana_pearent,true);

		set_box("moveable_tana", Vector3(49, -22, -2), Vector3(0), Vector3(5, 1, 1.1f), tana_color, tana_pearent,true);
		set_box("moveable_tana", Vector3(61, -22, -2), Vector3(0), Vector3(5, 1, 1.1f), tana_color, tana_pearent,true);

		set_box("moveable_tana", Vector3(55, -20, -2), Vector3(0), Vector3(5, 1, 0.9f), tana_color, tana_pearent, true);
	}

	{
		Vector4 ita_color = Vector4(169, 255, 169, 255) / 255.0f;
		Vector4 ita_color_off = Vector4(25, -20, 00, 0) / 255.0f;
		Gameobject* ita_pearent = Gameobject_manager::create("tana_pearent");
		//set_box("ita_pillar", Vector3(-66.0f, 25.5f, -0.5f), Vector3(0), Vector3(1, 1, 0.5f), stair_color, ita_pearent);
		//set_box("ita_pillar", Vector3(-77.5f, 25.5f, -0.5f), Vector3(0), Vector3(1, 1, 0.5f), stair_color, ita_pearent);

		//set_box("moveable_ita", Vector3(-69.32, 6.05, -2.11), Vector3(4.6f, 0, 0), Vector3(2, 23.1f, 0.25f), ita_color, ita_pearent,true);
		//set_box("moveable_ita", Vector3(-73.98, 6.05, -2.11), Vector3(4.6f, 0, 0), Vector3(2, 23.1f, 0.25f), ita_color, ita_pearent,true);

		set_gear("gear", Vector3(-54, 6, -2.2f), Vector3(0, 0, 0), Vector3(0.7f, 4.f, 1), ita_color + ita_color_off * 0, 9, ita_pearent);
		set_gear("gear", Vector3(-71, 15, -2.2f), Vector3(0, 0, 7), Vector3(0.7f, 4.f, 1),	  ita_color + ita_color_off * 1, 9, ita_pearent);
		set_gear("gear", Vector3(-75, 9, -12), Vector3(-6, 70, 25), Vector3(0.7f, 4, 1),	  ita_color + ita_color_off * 2, 9, ita_pearent);
		set_gear("gear", Vector3(-68, -8, -16), Vector3(-28.5f, 70, 90), Vector3(0.7f, 4, 1), ita_color + ita_color_off * 3, 9, ita_pearent);
		set_gear("gear", Vector3(-55, -21, -17), Vector3(-54, 71, 100), Vector3(0.7f, 4, 1),   ita_color + ita_color_off * 4, 9, ita_pearent);
	}

	//{
	//	Gameobject* door_box_pearent = Gameobject_manager::create("door_box_pearent");
	//	set_box("door",       Vector3(-76, -6.5f, 0), Vector3(0, 0, 0), Vector3(6.5f, 10.5f, 1), Vector4(73, 50, 50, 255) / 255.0f, door_box_pearent);
	//	set_box("door_totte", Vector3(-80.3f, -7.5f, -0.2f), Vector3(0, 0, 0), Vector3(0.7f, 0.7f, 1), Vector4(177, 180, 33, 255) / 255.0f, door_box_pearent);
	//}

	{
		Gameobject* pillar_pearent = Gameobject_manager::create("pillar_pearent");
		set_box("stair_part", Vector3(-0, 0, 6), Vector3(0, 0, 0), Vector3(3, 1, 3), stair_color, pillar_pearent);
		set_box("stair_part", Vector3(-0, 0, 6), Vector3(0, 0, 0), Vector3(3, 1, 3), stair_color, pillar_pearent);
		set_box("stair_part", Vector3(-0, 0, 6), Vector3(0, 0, 0), Vector3(3, 1, 3), stair_color, pillar_pearent);
		set_box("stair_part", Vector3(-0, 0, 6), Vector3(0, 0, 0), Vector3(3, 1, 3), stair_color, pillar_pearent);
		set_box("stair_part", Vector3(-0, 0, 6), Vector3(0, 0, 0), Vector3(3, 1, 3), stair_color, pillar_pearent);
		set_box("stair_part", Vector3(-0, 0, 6), Vector3(0, 0, 0), Vector3(3, 1, 3), stair_color, pillar_pearent);
		set_box("stair_part", Vector3(-0, 0, 6), Vector3(0, 0, 0), Vector3(3, 1, 3), stair_color, pillar_pearent);
	}

	{
		set_box("demno_part", Vector3(-0, 0, 6), Vector3(0, 0, 0), Vector3(3, 1, 3), stair_color);
		set_box("demno_part", Vector3(-0, 0, 6), Vector3(0, 0, 0), Vector3(3, 1, 3), stair_color);
		set_box("demno_part", Vector3(-0, 0, 6), Vector3(0, 0, 0), Vector3(3, 1, 3), stair_color);
		set_box("demno_part", Vector3(-0, 0, 6), Vector3(0, 0, 0), Vector3(3, 1, 3), stair_color);
		set_box("demno_part", Vector3(-0, 0, 6), Vector3(0, 0, 0), Vector3(3, 1, 3), stair_color);
		set_box("demno_part", Vector3(-0, 0, 6), Vector3(0, 0, 0), Vector3(3, 1, 3), stair_color);
		set_box("demno_part", Vector3(-0, 0, 6), Vector3(0, 0, 0), Vector3(3, 1, 3), stair_color);
	}

}

void Title_stage::start()
{

}

// 毎フレーム呼ばれる更新処理
void Title_stage::update()
{
	//if (input->getKeyTrigger(Key::Right)) {
	//	Scene_manager::set_active(Scenelist::scene_game);
	//	Scene_manager::set_inactive(Scenelist::scene_title);
	//}
}

// このスクリプトがアタッチされているGOのactiveSelfがtrueになった時呼ばれる
void Title_stage::onEnable()
{

}

// このスクリプトがアタッチされているGOのactiveSelfがfalseになった時呼ばれる
void Title_stage::onDisable()
{

}


