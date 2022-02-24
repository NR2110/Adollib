
#include "select_stage_manager.h"

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

void Select_state_manager::awake() {

	// select stage
	{
		// 各板の座標設定
		base_select_back_pos[0] = Vector3(16, +70, 0);
		base_select_back_pos[1] = Vector3(+8, +35, 0);
		base_select_back_pos[2] = Vector3(0, 0, 0);
		base_select_back_pos[3] = Vector3(+8, -35, 0);
		base_select_back_pos[4] = Vector3(16, -70, 0);

		// 読み込むtextureのpath
		const wchar_t* filepath[select_stage_size]{
			(L"./DefaultTexture/title/samune_stage01.png"),
			(L"./DefaultTexture/title/samune_stage02.png")
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
			select_stage_back[i] = Gameobject_manager::create(std::string("select_back") + std::to_string(i));
			select_stage_back[i]->transform->local_scale = Vector3(22, 14, 0.01f);
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

	// select player num
	{
		// 読み込むtextureのpath
		const wchar_t* filepath[select_stage_size]{
			(L"./DefaultTexture/title/player_soro.png"),
			(L"./DefaultTexture/title/player_duo.png")
		};

		// gameobjectの作成 & 親子関係の設定
		{
			select_playernum_sprite = Gameobject_manager::create(std::string("select_playernum_sprite_pearent"));
			gameobject->add_child(select_playernum_sprite);
			select_playernum_sprite->transform->local_pos = Vector3(-40, -15.5f, -23);

			for (int i = 0; i < 2; ++i) {
				auto sprite = Gameobject_manager::createPlane(std::string("sprite") + std::to_string(i));
				select_playernum_sprite->add_child(sprite);
				sprite->transform->local_scale = Vector3(11.91f, 3.8f, 1);
				sprite->transform->local_orient = quaternion_from_euler(0, 150 + 180 * i, 0);

				auto mat = Material_manager::create_material(std::string("select_playernum_material_") + std::to_string(i));
				mat->Load_VS("./DefaultShader/sprite_vs.cso");
				mat->Load_PS("./DefaultShader/sprite_ps.cso");
				mat->get_texture()->Load(filepath[i]);

				sprite->renderer->set_material(mat);
			}
		}
	}

	// yajirushi
	{

		for (int i = 0; i < 4; ++i) {
			yajirushi_go[i] = Gameobject_manager::create("yajirushi_p");
			gameobject->add_child(yajirushi_go[i]);
		}
		auto yajirushi_mat = Material_manager::create_material(std::string("select_yajirushi"));

		yajirushi_mat->Load_VS("./DefaultShader/sprite_vs.cso");
		yajirushi_mat->Load_PS("./DefaultShader/sprite_ps.cso");
		yajirushi_mat->get_texture()->Load(L"./DefaultTexture/title/title_yajirushi.png");
		{
			auto go = yajirushi_go[0];
			auto yajirushi = Gameobject_manager::createPlane(std::string("select_playernum_yajirushi_0"));
			go->add_child(yajirushi);
			yajirushi->transform->local_pos = Vector3(-28.12f, -13.93f, -18);
			yajirushi->transform->local_scale = Vector3(1.5f, 2, 1);
			yajirushi->transform->local_orient = quaternion_from_euler(0, 150, 82.4f);
			yajirushi->renderer->set_material(yajirushi_mat);
		}
		{
			auto go = yajirushi_go[1];
			auto yajirushi = Gameobject_manager::createPlane(std::string("select_playernum_yajirushi_1"));
			go->add_child(yajirushi);
			yajirushi->transform->local_pos = Vector3(-49.57f, -15.48f, -29.25f);
			yajirushi->transform->local_scale = Vector3(1.5f, 2, 1);
			yajirushi->transform->local_orient = quaternion_from_euler(0, 150, -95.1f);
			yajirushi->renderer->set_material(yajirushi_mat);
		}
		{
			auto go = yajirushi_go[2];
			auto yajirushi = Gameobject_manager::createPlane(std::string("select_playernum_yajirushi_2"));
			go->add_child(yajirushi);
			yajirushi->transform->local_pos = Vector3(-15.7f, 11.23f, -18);
			yajirushi->transform->local_scale = Vector3(2.2f, 2, 1);
			yajirushi->transform->local_orient = quaternion_from_euler(0, 150, -7.4f);
			yajirushi->renderer->set_material(yajirushi_mat);
		}
		{
			auto go = yajirushi_go[3];
			auto yajirushi = Gameobject_manager::createPlane(std::string("select_playernum_yajirushi_3"));
			go->add_child(yajirushi);
			yajirushi->transform->local_pos = Vector3(-15.52f, -11.43f, -18);
			yajirushi->transform->local_scale = Vector3(2.2f, 2, 1);
			yajirushi->transform->local_orient = quaternion_from_euler(0, 150, -171.3f);
			yajirushi->renderer->set_material(yajirushi_mat);
		}

	}

}

void Select_state_manager::start() {
	title_state_manager = Gameobject_manager::find("title_state_manager")->findComponent<Title_state_manager>();
	title_state_manager->add_statebase(this);

	//player_manager = Gameobject_manager::find("Player_manager",Scenelist::scene_player)->findComponent<Player_manager>();
	//player_manager->set_moveable(false);
	//player_manager->set_Tpause_and_set_transform(Vector3(0,-1000,-70),quaternion_identity());

	//Gameobject_manager::find("camera", Scenelist::scene_player)->findComponent<Camera_component>()->gameobject->is_active = false;

}

void Select_state_manager::update() {


	yajirushi_timer += time->deltaTime() * 3;

	Vector3 x_vec = Vector3(1, 0.1f, 0).unit_vect();
	yajirushi_go[0]->transform->local_pos = x_vec * +(cosf(yajirushi_timer) * 0.5f - 0.5f);
	yajirushi_go[1]->transform->local_pos = x_vec * -(cosf(yajirushi_timer) * 0.5f + 0.5f);

	Vector3 y_vec = Vector3(0, 1, 0).unit_vect();
	yajirushi_go[2]->transform->local_pos = y_vec * +(cosf(yajirushi_timer - 0.8f) * 0.5f + 0.5f);
	yajirushi_go[3]->transform->local_pos = y_vec * -(cosf(yajirushi_timer - 0.8f) * 0.5f - 0.5f);





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
	if (state == Title_state_B::Update_1) {

	}

	if (state == Title_state_B::Update_2) {

		// select stage
		{
			// materialのセット
			for (int i = 0; i < 5; ++i) {
				int now_id = i;
				int material_num = (select_stage_size + 2 - now_id + select_stage_num) % select_stage_size;
				select_stage_front[now_id]->renderer->set_material(select_stage_material[material_num]);
			}

			// 入力をとる
			if (change_select_sign == 0) {
				change_select_timer = 0;
				if (input->getKeyTrigger(Key::Up) || input->getPadTrigger(0, GamePad::UP))    change_select_sign = 1;
				if (input->getKeyTrigger(Key::Down) || input->getPadTrigger(0, GamePad::DOWN))change_select_sign = -1;
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
					if (input->getKeyState(Key::Up) || input->getPadState(0, GamePad::UP))change_select_state_sign = 1;
					if (input->getKeyState(Key::Down) || input->getPadState(0, GamePad::DOWN))change_select_state_sign = -1;
					change_timer_pow = 5;
					is_next_state = false;
				}
			}

		}

		// select player num
		{
			// 入力をとる
			if (change_playernum_sign == 0) {
				change_playernum_timer = 0;
				if (input->getKeyTrigger(Key::Left) || input->getPadTrigger(0, GamePad::LEFT))  change_playernum_sign = +1;
				if (input->getKeyTrigger(Key::Right) || input->getPadTrigger(0, GamePad::RIGHT)) change_playernum_sign = -1;
				if (change_playernum_sign != 0)
					select_player_num = 1 - select_player_num;

				change_playernum_timer = 0;
			}

			float rotate_angle = 180 * select_player_num;
			// 入力が無い時
			if (change_playernum_sign == 0) {
			}

			// 適当にeasingで回転させる
			else
			{
				change_playernum_timer += time->deltaTime() * 5; //0 ~ 1
				float t = change_playernum_timer - 1;
				float q_func = (-1 * t * t + 1); //適当なeasing用2次関数 (0<q<1)

				rotate_angle = (180 * select_player_num) + change_playernum_sign * 180 * 1 * (1 - q_func);

				if (change_playernum_timer > 1)change_playernum_sign = 0;
			}

			select_playernum_sprite->transform->local_orient = quaternion_from_euler(0, rotate_angle, 0);
		}




		if (input->getKeyTrigger(Key::Enter) || input->getPadTrigger(0, GamePad::A))is_next_state = true;

		if (is_next_state && change_select_sign == 0 && change_playernum_sign == 0) {
			//player_manager->set_Tpause_and_set_transform(Vector3(0, -50, -70), quaternion_identity());
			//player_manager->set_moveable(true);

			title_state_manager->set_nextstate_B(Title_state_B::Update_3);
		}
	}

	if (state == Title_state_B::Update_3) {


		Gameobject_manager::find("Player_manager", Scenelist::scene_player)->findComponent<Player_manager>()->add_players(select_player_num + 1);

		Scene_manager::set_active(Scenelist::scene_game);
		Gameobject_manager::find("Stage_manager", Scenelist::scene_game)->findComponent<Stage_manager>()->set_next_stage_type(static_cast<Stage_types>(select_stage_num));

		Scene_manager::set_inactive(Scenelist::scene_title);
	}

	if (state == Title_state_B::Update_4) {

	}




}