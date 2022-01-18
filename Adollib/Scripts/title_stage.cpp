
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
	wall_color =  Vector4(234,234,234,255) / 255.0f;
	floar_color = Vector4(57, 34, 34, 255) / 255.0f;
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
		set_box("Floar", Vector3(0, -33, 0), Vector3(0), Vector3(100, 1, 1), floar_color);
	}

	{
		Gameobject* stair_pearent = Gameobject_manager::create("stair_pearent");
		set_box("stair_part", Vector3(-78, -18, -3), Vector3(-1,0,0), Vector3(15, 1, 3), stair_color, stair_pearent);
		set_box("stair_part", Vector3(-58, -20, -3), Vector3(1,0,0), Vector3(3, 1, 3), stair_color, stair_pearent);
		set_box("stair_part", Vector3(-50, -22, -3), Vector3(1,0,0), Vector3(3, 1, 3), stair_color, stair_pearent);
		set_box("stair_part", Vector3(-42, -24, -3), Vector3(1,0,0), Vector3(3, 1, 3), stair_color, stair_pearent);
		set_box("stair_part", Vector3(-34, -26, -3), Vector3(1,0,0), Vector3(3, 1, 3), stair_color, stair_pearent);
		set_box("stair_part", Vector3(-26, -28, -3), Vector3(1,0,0), Vector3(3, 1, 3), stair_color, stair_pearent);
		set_box("stair_part", Vector3(-18, -30, -3), Vector3(1,0,0), Vector3(3, 1, 3), stair_color, stair_pearent);
	}

	{
		Gameobject* trash_box_pearent = Gameobject_manager::create("trash_box_pearent");
		set_box("trash_box", Vector3(54, -26.5f, -3.5f), Vector3(0, 4.6f, 0), Vector3(13, 5.5f, 3),      Vector4(195, 195, 195, 255) / 255.0f, trash_box_pearent);
		set_box("trash_box", Vector3(53.5f, -21, -4.0f), Vector3(-23.5f, 4.6f, 0), Vector3(14, 1, 4), Vector4(0, 94, 42, 255) / 255.0f, trash_box_pearent);
	}

	{
		Gameobject* door_box_pearent = Gameobject_manager::create("door_box_pearent");
		set_box("door",       Vector3(-76, -6.5f, 0), Vector3(0, 0, 0), Vector3(6.5f, 10.5f, 1), Vector4(73, 50, 50, 255) / 255.0f, door_box_pearent);
		set_box("door_totte", Vector3(-80.3f, -7.5f, -0.2f), Vector3(0, 0, 0), Vector3(0.7f, 0.7f, 1), Vector4(177, 180, 33, 255) / 255.0f, door_box_pearent);
	}

	{
		Gameobject* pillar_pearent = Gameobject_manager::create("pillar_pearent");
		set_box("stair_part", Vector3(-0,0,6), Vector3(0, 0, 0), Vector3(3, 1, 3), stair_color, pillar_pearent);
		set_box("stair_part", Vector3(-0,0,6), Vector3(0, 0, 0), Vector3(3, 1, 3), stair_color, pillar_pearent);
		set_box("stair_part", Vector3(-0,0,6), Vector3(0, 0, 0), Vector3(3, 1, 3), stair_color, pillar_pearent);
		set_box("stair_part", Vector3(-0,0,6), Vector3(0, 0, 0), Vector3(3, 1, 3), stair_color, pillar_pearent);
		set_box("stair_part", Vector3(-0,0,6), Vector3(0, 0, 0), Vector3(3, 1, 3), stair_color, pillar_pearent);
		set_box("stair_part", Vector3(-0,0,6), Vector3(0, 0, 0), Vector3(3, 1, 3), stair_color, pillar_pearent);
		set_box("stair_part", Vector3(-0,0,6), Vector3(0, 0, 0), Vector3(3, 1, 3), stair_color, pillar_pearent);
	}

}

void Title_stage::start()
{

}

// 毎フレーム呼ばれる更新処理
void Title_stage::update()
{
	if(input->getKeyTrigger(Key::Right)) {
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


