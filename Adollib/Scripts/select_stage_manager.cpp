
#include "select_stage_manager.h"

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

#include "title_state_manager.h"

using namespace Adollib;

void Select_state_manager::awake() {

	// 各板の座標設定
	base_select_back_pos[0] = Vector3(16, +70, 0);
	base_select_back_pos[1] = Vector3(+8, +35, 0);
	base_select_back_pos[2] = Vector3(0, 0, 0);
	base_select_back_pos[3] = Vector3(+8, -35, 0);
	base_select_back_pos[4] = Vector3(16, -70, 0);

	// 読み込むtextureのpath
	const wchar_t* filepath[select_stage_size]{
		(L"./DefaultTexture/number/debug_01.png"),
		(L"./DefaultTexture/number/debug_02.png"),
		(L"./DefaultTexture/number/debug_03.png"),
		(L"./DefaultTexture/number/debug_04.png"),
		(L"./DefaultTexture/number/debug_05.png")
	};

	// selectstageに張るmaterial作成
	for (int i = 0; i < select_stage_size; ++i) {
		select_stage_material[i] = Material_manager::create_material(std::string("select_stage_material_") + std::to_string(i));

		select_stage_material[i]->Load_VS("./DefaultShader/sprite_vs.cso");
		select_stage_material[i]->Load_PS("./DefaultShader/sprite_ps.cso");

		select_stage_material[i]->get_texture()->Load(filepath[i]);
	}

	// gameobjectの作成 & 親子関係の設定
	for (int i = 0; i < 5; ++i) {
		select_stage_back[i] = Gameobject_manager::createCube(std::string("select_back") + std::to_string(i));
		select_stage_back[i]->transform->local_scale = Vector3(22, 14, 0.01f);
		auto material = Material_manager::create_material("select_back_material");
		material->Load_VS("./DefaultShader/default_vs.cso");
		material->Load_PS("./DefaultShader/default_ps_originalcolor.cso");
		select_stage_back[i]->renderer->set_material(material);
		select_stage_back[i]->renderer->color = Vector4(246, 237, 240, 255) / 255.0f;
		gameobject->add_child(select_stage_back[i]);

		select_stage_front[i] = Gameobject_manager::createPlane(std::string("select_front") + std::to_string(i));
		select_stage_front[i]->transform->local_orient = quaternion_from_euler(0, 180, 0);
		select_stage_front[i]->transform->local_pos = Vector3(0, 0, -2);
		select_stage_back[i]->add_child(select_stage_front[i]);
	}

	// selectstageの座標
	gameobject->transform->local_pos = Vector3(32, -1, 90);
	gameobject->transform->local_orient = quaternion_from_euler(Vector3(-0.5f, 30, 0));
}

void Select_state_manager::start() {
	title_state_manager = Gameobject_manager::find("title_state_manager")->findComponent<Title_state_manager>();
	title_state_manager->add_statebase(this);
}

void Select_state_manager::update() {






}



void Select_state_manager::start_state(Title_state_B state) {

}

void Select_state_manager::select_state(Title_state_B state) {

	if (state == Title_state_B::Awake) {
		// materialのセット
		for (int i = 0; i < 5; ++i) {
			int now_id = i;
			int material_num = (select_stage_size + 2 - now_id + select_stage_num) % select_stage_size;
			select_stage_front[now_id]->renderer->set_material(select_stage_material[material_num]);
		}
		// 座標調整
		{
			for (int i = 0; i < 5; ++i) {
				select_stage_back[i]->transform->local_pos = base_select_back_pos[i];
			}
			change_timer_pow = 3;
		}
	}

	if (state == Title_state_B::Update_2) {

		// materialのセット
		for (int i = 0; i < 5; ++i) {
			int now_id = i;
			int material_num = (select_stage_size + 2 - now_id + select_stage_num) % select_stage_size;
			select_stage_front[now_id]->renderer->set_material(select_stage_material[material_num]);
		}

		// 入力をとる
		if (change_select_sign == 0) {
			change_select_timer = 0;
			if (input->getKeyTrigger(Key::Up))change_select_sign = 1;
			if (input->getKeyTrigger(Key::Down))change_select_sign = -1;
		}

		// 何も入力が無い時の座標調整
		if (change_select_state_sign == 0 && change_select_sign == 0) {

			for (int i = 0; i < 5; ++i) {
				select_stage_back[i]->transform->local_pos = base_select_back_pos[i];
			}

			change_timer_pow = 3;
		}

		// 入力があった場合 適当なeasingで動かす
		else {
			change_select_timer += time->deltaTime() * change_timer_pow;
			if (change_select_timer > 1)change_select_timer = 1; //1以上にならないように

			float t = change_select_timer - 1;
			float q_func = (-1 * t * t + 1); //適当なeasing用2次関数 (0<q<1)

			for (int i = 0; i < 5; ++i) {
				int now_id = i;
				if (now_id + change_select_sign < 0 || now_id + change_select_sign >= 5) {
					select_stage_back[now_id]->transform->local_pos = Vector3(10000, 10000, 10000);
					continue;
				}
				int next_id = (now_id + change_select_sign + 5) % 5;

				select_stage_back[now_id]->transform->local_pos = base_select_back_pos[now_id] + (base_select_back_pos[next_id] - base_select_back_pos[now_id]) * q_func;
			}

			if (change_select_timer == 1) {
				select_stage_num = (select_stage_num + change_select_sign + select_stage_size) % select_stage_size;
				change_select_sign = change_select_state_sign;
				change_select_state_sign = 0;
				change_select_timer = 0;
			}


			if (change_select_timer > 0.8f) {
				if (input->getKeyState(Key::Up))change_select_state_sign = 1;
				if (input->getKeyState(Key::Down))change_select_state_sign = -1;
				change_timer_pow = 5;
			}
		}



		if (input->getKeyTrigger(Key::Enter))is_next_state = true;

		//if (is_next_state && change_select_sign == 0) title_state_manager->set_nextstate_B(Title_state_B::Update_3);
		if (is_next_state && change_select_sign == 0) {
			Scene_manager::set_active(Scenelist::scene_game);
			Scene_manager::set_inactive(Scenelist::scene_title);
		}
	}




}