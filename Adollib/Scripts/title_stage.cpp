
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

#include "../Adollib/Imgui/imgui.h"

#include "../Adollib/Scripts/Physics/ALP__physics_manager.h"

using namespace Adollib;

void set_box(const char* name, Vector3 position, Vector3 rotate, Vector3 scale, Vector4 color, Gameobject* pearent = nullptr) {
	auto go = Gameobject_manager::createCube(name);
	go->transform->local_scale = scale;
	go->transform->local_orient = quaternion_from_euler(rotate);
	go->transform->local_pos = position;
	go->renderer->color = color;

	if (pearent != nullptr)pearent->add_child(go);
}

// 所属するシーンの初期化時に一度だけ呼ばれる
void Title_stage::awake()
{
	wall_color = Vector4(234, 234, 234, 255) / 255.0f;
	//floar_color = Vector4(57, 34, 34, 255) / 255.0f;
	floar_color = Vector4(179, 179, 179, 255) / 255.0f;
	stair_color = Vector4(64, 64, 64, 255) / 255.0f;

	{
		auto title = Gameobject_manager::createPlane("Title");
		title->transform->local_pos = Vector3(0, 3.6f, -0.1f);
		title->transform->local_orient = quaternion_from_euler(0, 180, 0);
		title->transform->local_scale = Vector3(75, 35, 0);
		auto title_material = Material_manager::create_material("Title");
		title_material->get_texture()->Load(L"./DefaultTexture/title/title_rogo.png");
		title_material->BS_state = State_manager::BStypes::BS_ALPHA;
		title_material->Load_VS("./DefaultShader/sprite_vs.cso");
		title_material->Load_PS("./DefaultShader/sprite_ps.cso");
		title->renderer->set_material(title_material);
	}
	{
		set_box("Wall", Vector3(0, 0, 1), Vector3(0), Vector3(100, 100, 1), wall_color);
		set_box("Floar", Vector3(0, -33, -3), Vector3(0), Vector3(100, 1, 3), floar_color);
	}

	{
		Gameobject* stair_pearent = Gameobject_manager::create("stair_pearent");
		set_box("stair_part", Vector3(-78, -18, -3), Vector3(-1, 0, 0), Vector3(15, 1, 3), stair_color, stair_pearent);
		set_box("stair_part", Vector3(-58, -20, -3), Vector3(1, 0, 0), Vector3(3, 1, 3), stair_color, stair_pearent);
		set_box("stair_part", Vector3(-50, -22, -3), Vector3(1, 0, 0), Vector3(3, 1, 3), stair_color, stair_pearent);
		set_box("stair_part", Vector3(-42, -24, -3), Vector3(1, 0, 0), Vector3(3, 1, 3), stair_color, stair_pearent);
		set_box("stair_part", Vector3(-34, -26, -3), Vector3(1, 0, 0), Vector3(3, 1, 3), stair_color, stair_pearent);
		set_box("stair_part", Vector3(-26, -28, -3), Vector3(1, 0, 0), Vector3(3, 1, 3), stair_color, stair_pearent);
		set_box("stair_part", Vector3(-18, -30, -3), Vector3(1, 0, 0), Vector3(3, 1, 3), stair_color, stair_pearent);
	}

	{
		Gameobject* cloth_pearent = Gameobject_manager::create("cloth_pearent");
		set_box("cloth_pillar", Vector3(-20, 0, -4), Vector3(-35,0,0), Vector3(1, 1, 5), Vector4(195, 195, 195, 255) / 255.0f, cloth_pearent);
		set_box("cloth_pillar", Vector3(+20, 0, -4), Vector3(-35,0,0), Vector3(1, 1, 5), Vector4(195, 195, 195, 255) / 255.0f, cloth_pearent);
		{
			auto croth = Gameobject_manager::createFromFBX("croth", "./DefaultModel/plane_16x16vertex.fbx");
			//croth->transform->local_scale = Vector3(20, 4, 10);
			croth->transform->local_scale = Vector3(20, 20, 20);
			croth->transform->local_pos = Vector3(0, 0, -5);
			croth->transform->local_orient = quaternion_from_euler(Vector3(55, 0, 0));
			//croth->transform->local_orient = quaternion_from_euler(Vector3(90, 0, 0));

			auto coll = croth->addComponent<Collider_Croth>();
			//coll->default_physics_data.is_moveable = false;
			coll->ignore_tags = Collider_tags::Stage;
			coll->load_file("./DefaultModel/plane_16x16vertex.fbx", true, false);
			croth->transform->local_scale = Vector3(19, 20, 20);
			{
				Physics_data data;

				//for (int i = 259; i < 275; ++i) {
				//	data = coll->get_vertex_data(0, i);
				//	data.is_moveable = false;
				//	coll->set_vertex_data(0, i, data);
				//}

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
			renderer->color = Vector4(1, 0, 0, 1);

			cloth_pearent->add_child(croth);
		}

			cloth_pearent->transform->local_pos = Vector3(58, 18, 0);
			cloth_pearent->transform->local_orient = quaternion_from_euler(Vector3(0, 0, 14));
	}
	{
		Vector4 tana_color = Vector4(254, 159, 159, 255) / 255.0f;
		Gameobject* tana_pearent = Gameobject_manager::create("tana_pearent");
		set_box("tana", Vector3(55, -28, -2), Vector3(0), Vector3(30, 1, 2), stair_color, tana_pearent);
		set_box("tana", Vector3(82, -30, -1), Vector3(0), Vector3(0.50f, 2, 0.50f), stair_color, tana_pearent);
		set_box("tana", Vector3(28, -30, -1), Vector3(0), Vector3(0.75f, 2, 0.75f), stair_color, tana_pearent);
		set_box("moveable_tana", Vector3(37, -26, -2), Vector3(0), Vector3(5, 1, 1.5f), tana_color, tana_pearent);
		set_box("moveable_tana", Vector3(49, -26, -2), Vector3(0), Vector3(5, 1, 1.5f), tana_color, tana_pearent);
		set_box("moveable_tana", Vector3(61, -26, -2), Vector3(0), Vector3(5, 1, 1.5f), tana_color, tana_pearent);
		set_box("moveable_tana", Vector3(73, -26, -2), Vector3(0), Vector3(5, 1, 1.5f), tana_color, tana_pearent);

		set_box("moveable_tana", Vector3(43, -24, -2), Vector3(0), Vector3(5, 1, 1.3f), tana_color, tana_pearent);
		set_box("moveable_tana", Vector3(55, -24, -2), Vector3(0), Vector3(5, 1, 1.3f), tana_color, tana_pearent);
		set_box("moveable_tana", Vector3(67, -24, -2), Vector3(0), Vector3(5, 1, 1.3f), tana_color, tana_pearent);

		set_box("moveable_tana", Vector3(49, -22, -2), Vector3(0), Vector3(5, 1, 1.1f), tana_color, tana_pearent);
		set_box("moveable_tana", Vector3(61, -22, -2), Vector3(0), Vector3(5, 1, 1.1f), tana_color, tana_pearent);

		set_box("moveable_tana", Vector3(55, -20, -2), Vector3(0), Vector3(5, 1, 0.9f), tana_color, tana_pearent);
	}

	{
		Vector4 ita_color = Vector4(190, 218, 172, 255) / 255.0f;
		Gameobject* ita_pearent = Gameobject_manager::create("tana_pearent");
		set_box("ita_pillar", Vector3(-66.0f, 25.5f, -0.5f), Vector3(0), Vector3(1, 1, 0.5f), stair_color, ita_pearent);
		set_box("ita_pillar", Vector3(-77.5f, 25.5f, -0.5f), Vector3(0), Vector3(1, 1, 0.5f), stair_color, ita_pearent);

		set_box("moveable_ita", Vector3(-69.32, 6.05, -2.11), Vector3(4.6f, 0, 0), Vector3(2, 23.1f, 0.25f), ita_color, ita_pearent);
		set_box("moveable_ita", Vector3(-73.98, 6.05, -2.11), Vector3(4.6f, 0, 0), Vector3(2, 23.1f, 0.25f), ita_color, ita_pearent);
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
	if (input->getKeyTrigger(Key::Right)) {
		Scene_manager::set_active(Scenelist::scene_game);
		Scene_manager::set_inactive(Scenelist::scene_title);

	}
}

// このスクリプトがアタッチされているGOのactiveSelfがtrueになった時呼ばれる
void Title_stage::onEnable()
{

}

// このスクリプトがアタッチされているGOのactiveSelfがfalseになった時呼ばれる
void Title_stage::onDisable()
{

}


