
#include "select_playernum_manager.h"

#include "../Adollib/Scripts/Main/systems.h"
#include "../Adollib/Scripts/Main/Adollib.h"
#include "../Adollib/Scripts/Imgui/debug.h"
#include "../Adollib/Scripts/Object/component_camera.h"
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

#include "player_manager.h"
#include "stage_manager.h"
#include "title_state_manager.h"

using namespace Adollib;

void Select_playernum_manager::awake() {

	// 各板の座標設定
	base_select_back_pos = Vector3(-40, -15.5f, -23);

	// 読み込むtextureのpath
	const wchar_t* filepath[select_stage_size]{
		(L"./DefaultTexture/title/player_soro.png"),
		(L"./DefaultTexture/title/player_duo.png")
	};

	// selectstageに張るmaterial作成
	for (int i = 0; i < select_stage_size; ++i) {
		select_stage_material[i] = Material_manager::create_material(std::string("select_playernum_material_") + std::to_string(i));

		select_stage_material[i]->Load_VS("./DefaultShader/sprite_vs.cso");
		select_stage_material[i]->Load_PS("./DefaultShader/sprite_ps.cso");

		select_stage_material[i]->get_texture()->Load(filepath[i]);
	}

	// gameobjectの作成 & 親子関係の設定
	{
		select_sprite = Gameobject_manager::create(std::string("select_playernum_sprite_pearent"));
		gameobject->add_child(select_sprite);
		select_sprite->transform->local_pos = base_select_back_pos;

		for (int i = 0; i < 2; ++i) {
			auto sprite = Gameobject_manager::createPlane(std::string("sprite") + std::to_string(i));
			select_sprite->add_child(sprite);
			sprite->transform->local_scale = Vector3(11.91f, 3.8f, 1);
			sprite->transform->local_orient = quaternion_from_euler(0, 150 + 180 * i, 0);
			sprite->renderer->set_material(select_stage_material[i]);
		}
	}

}

void Select_playernum_manager::start() {
	title_state_manager = Gameobject_manager::find("title_state_manager")->findComponent<Title_state_manager>();
	title_state_manager->add_statebase(this);

}

void Select_playernum_manager::update() {



}


void Select_playernum_manager::start_state(Title_state_B state) {

}

void Select_playernum_manager::select_state(Title_state_B state) {

	if (state == Title_state_B::Awake) {

	}

	if (state == Title_state_B::Update_1) {

	}

	if (state == Title_state_B::Update_2) {

		// 入力をとる
		if (change_select_sign == 0) {
			change_select_timer = 0;
			if (input->getKeyTrigger(Key::Left) || input->getPadTrigger(0, GamePad::LEFT))  change_select_sign = +1;
			if (input->getKeyTrigger(Key::Right) || input->getPadTrigger(0, GamePad::RIGHT)) change_select_sign = -1;
			if (change_select_sign != 0)
				select_player_num = 1 - select_player_num;

			change_select_timer = 0;
		}

		// 入力が無い時
		if (change_select_sign == 0) {
			rotate_angle = 180 * select_player_num;
		}

		// 適当にeasingで回転させる
		else
		{
			change_select_timer += time->deltaTime() * 5; //0 ~ 1
			float t = change_select_timer - 1;
			float q_func = (-1 * t * t + 1); //適当なeasing用2次関数 (0<q<1)

			rotate_angle = (180 * select_player_num) + change_select_sign * 180 * 1 * (1 - q_func);

			if (change_select_timer > 1)change_select_sign = 0;
		}

		select_sprite->transform->local_orient = quaternion_from_euler(0, rotate_angle, 0);

	}

	if (state == Title_state_B::Update_3) {

		if (change_select_sign != 0) {
			title_state_manager->set_nextstate_B(Title_state_B::Update_2);
		}
		else {
			title_state_manager->set_nextstate_B(Title_state_B::Update_4);
		}
	}




}