
#include "tutrial_stage01_move_camera_catch.h"

#include "../Adollib/Scripts/Object/gameobject_manager.h"
#include "../Adollib/Scripts/Scene/scene_manager.h"

#include "../Adollib/Scripts/Math/closest_func.h"
#include "../Adollib/Scripts/Imgui/imgui_all.h"

#include "../Adollib/Scripts/Imgui/debug.h"
#include "../Adollib/Scripts/Physics/ray.h"
#include "../Adollib/Scripts/Physics/joint.h"
#include "../Adollib/Scripts/Physics/collider_croth.h"
#include "../Adollib/Scripts/Physics/ALP__physics_manager.h"

#include "../Adollib/Scripts/Renderer/mesh_renderer.h"
#include "../Adollib/Scripts/Renderer/material_manager.h"
#include "../Adollib/Scripts/Renderer/material.h"
#include "../Adollib/Scripts/Renderer/texture.h"
#include "../Adollib/Scripts/Renderer/sprite_renderer.h"
#include "../Adollib/Scripts/Renderer/UI_renderer.h"
#include "../Adollib/Scripts/Object/component_camera.h"

#include "../Adollib/Scripts/Main/systems.h"
#include "../Adollib/Scripts/Renderer/croth_renderer.h"

#include "input_changer.h"
#include "player.h"
#include "stage_manager.h"
#include "stage_base.h"

using namespace Adollib;

void Tutrial_stage01_move_camera_catch::awake() {

	// materialの作成
	{
		mat_tutrial_check = Material_manager::create_material("mat_tutrial_check");
		mat_tutrial_check->get_texture()->Load(L"./DefaultTexture/tutrial/tutrial_check.png");
		mat_tutrial_check->BS_state = State_manager::BStypes::BS_ALPHA;
		mat_tutrial_check->Load_VS("./DefaultShader/ui_vs.cso");
		mat_tutrial_check->Load_PS("./DefaultShader/ui_ps.cso");
		mat_tutrial_check->is_render_shadow = false;

		mat_tutrial_move = Material_manager::create_material("mat_tutrial_move");
		mat_tutrial_move->get_texture()->Load(L"./DefaultTexture/tutrial/tutrial_stickL_move.png");
		mat_tutrial_move->BS_state = State_manager::BStypes::BS_ALPHA;
		mat_tutrial_move->Load_VS("./DefaultShader/ui_vs.cso");
		mat_tutrial_move->Load_PS("./DefaultShader/ui_ps.cso");
		mat_tutrial_move->is_render_shadow = false;

		mat_tutrial_camera = Material_manager::create_material("mat_tutrial_camera");
		mat_tutrial_camera->get_texture()->Load(L"./DefaultTexture/tutrial/tutrial_stickR_camera.png");
		mat_tutrial_camera->BS_state = State_manager::BStypes::BS_ALPHA;
		mat_tutrial_camera->Load_VS("./DefaultShader/ui_vs.cso");
		mat_tutrial_camera->Load_PS("./DefaultShader/ui_ps.cso");
		mat_tutrial_camera->is_render_shadow = false;

		mat_tutrial_hand = Material_manager::create_material("mat_tutrial_hand");
		mat_tutrial_hand->get_texture()->Load(L"./DefaultTexture/tutrial/tutrial_LTRL_hand.png");
		mat_tutrial_hand->BS_state = State_manager::BStypes::BS_ALPHA;
		mat_tutrial_hand->Load_VS("./DefaultShader/ui_vs.cso");
		mat_tutrial_hand->Load_PS("./DefaultShader/ui_ps.cso");
		mat_tutrial_hand->is_render_shadow = false;

		mat_tutrial_catch_object = Material_manager::create_material("mat_tutrial_catch_object");
		mat_tutrial_catch_object->get_texture()->Load(L"./DefaultTexture/tutrial/tutrial_catch_object.png");
		mat_tutrial_catch_object->BS_state = State_manager::BStypes::BS_ALPHA;
		mat_tutrial_catch_object->Load_VS("./DefaultShader/ui_vs.cso");
		mat_tutrial_catch_object->Load_PS("./DefaultShader/ui_ps.cso");
		mat_tutrial_catch_object->is_render_shadow = false;
	}

	// gameobjectの作成
	{
		Vector3 local_check_pos = Vector3(5.15f, 2.9f, 1);
		check_base_scale = Vector3(0.21f, 1.5f, 1);

		{
			go_tutrial_move = Gameobject_manager::createPlane("go_tutrial_move");
			auto renderer = go_tutrial_move->addComponent<UI_renderer>();
			renderer->set_material(mat_tutrial_move);
			go_tutrial_move->transform->local_scale = Vector3(7, 1, 1);

			auto check = Gameobject_manager::createPlane("check");
			auto check_renderer = check->addComponent<UI_renderer>();
			check_renderer->set_material(mat_tutrial_check);
			check_renderer->depth = 1;
			check->transform->local_scale = check_base_scale;
			check->transform->local_pos = local_check_pos;
			go_tutrial_move->add_child(check);

		}
		{
			go_tutrial_camera = Gameobject_manager::createPlane("go_tutrial_camera");
			auto renderer = go_tutrial_camera->addComponent<UI_renderer>();
			renderer->set_material(mat_tutrial_camera);
			go_tutrial_camera->transform->local_scale = Vector3(7, 1, 1);

			auto check = Gameobject_manager::createPlane("check");
			auto check_renderer = check->addComponent<UI_renderer>();
			check_renderer->set_material(mat_tutrial_check);
			check_renderer->depth = 1;
			check->transform->local_scale = check_base_scale;
			check->transform->local_pos = local_check_pos;
			go_tutrial_camera->add_child(check);
		}
		{
			go_tutrial_hand = Gameobject_manager::createPlane("go_tutrial_hand");
			auto renderer = go_tutrial_hand->addComponent<UI_renderer>();
			renderer->set_material(mat_tutrial_hand);
			go_tutrial_hand->transform->local_scale = Vector3(7, 1, 1);

			auto check = Gameobject_manager::createPlane("check");
			auto check_renderer = check->addComponent<UI_renderer>();
			check_renderer->set_material(mat_tutrial_check);
			check_renderer->depth = 1;
			check->transform->local_scale = check_base_scale;
			check->transform->local_pos = local_check_pos;
			go_tutrial_hand->add_child(check);
		}
		{
			go_tutrial_catch_object = Gameobject_manager::createPlane("go_tutrial_catch_object");
			auto renderer = go_tutrial_catch_object->addComponent<UI_renderer>();
			renderer->set_material(mat_tutrial_catch_object);
			go_tutrial_catch_object->transform->local_scale = Vector3(7, 1, 1);

			auto check = Gameobject_manager::createPlane("check");
			auto check_renderer = check->addComponent<UI_renderer>();
			check_renderer->set_material(mat_tutrial_check);
			check_renderer->depth = 1;
			check->transform->local_scale = check_base_scale;
			check->transform->local_pos = local_check_pos;
			go_tutrial_catch_object->add_child(check);
		}

	}

	tutrial_flag = -1;
}

void Tutrial_stage01_move_camera_catch::update() {

	tutrial_timer += time->deltaTime();

	// とりあえず非表示に 表示する際は毎フレームtrueにしてる場所があるので問題ない
	go_tutrial_move->is_active = false;
	go_tutrial_camera->is_active = false;
	go_tutrial_hand->is_active = false;
	go_tutrial_catch_object->is_active = false;
	for (auto& child : *go_tutrial_move->children())child->is_active = false;
	for (auto& child : *go_tutrial_camera->children())child->is_active = false;
	for (auto& child : *go_tutrial_hand->children())child->is_active = false;
	for (auto& child : *go_tutrial_catch_object->children())child->is_active = false;

	// moveとcameraのtutrial
	// set_tutrial_move_and_camera_flagは初期値-1 条件で0になり、変化していく
	if (tutrial_flag != -1) {
		tutrial_move_and_camera();
		tutrial_hand_stretch();
		tutrial_hand_catch();
	}

	// stageが無ければとりあえず初期化 & return
	if (player->get_stage_manager_ptr() == nullptr) {
		tutrial_flag = -1;
		return;
	};

	// stageがchangeされたらflagを初期化
	if (player->get_stage_manager_ptr()->is_changed_stage == true)tutrial_flag = -1;

	// set_tutrial_move_and_camera_flagがまだセットされていない &&
	// 現在のstageがstage_1である &&
	// stageが開始されて5秒立った
	if (
		tutrial_flag == -1 &&
		player->get_stage_manager_ptr()->now_stage == Stage_types::stage_1 &&
		player->get_stage_manager_ptr()->get_nowstage_timer() > 5
		)
		tutrial_flag = 0;

}

void Tutrial_stage01_move_camera_catch::tutrial_move_and_camera() {

	Gameobject* sprites[2]{
		go_tutrial_move,
		go_tutrial_camera
	};

	sprites[0]->transform->local_pos.y = 80;
	sprites[1]->transform->local_pos.y = 55;

	sprites[0]->is_active = true;
	sprites[1]->is_active = true;


	if (tutrial_flag == 0) {
		tutrial_flag++;
		tutrial_timer = 0;
	}

	// 左から出てくる
	if (tutrial_flag == 1) {
		const float move_x_timer = ALClamp(tutrial_timer * 5, 0, 1);
		const float camera_x_timer = ALClamp(tutrial_timer * 5 - 0.2f, 0, 1);
		const float move_x_pos = -200 + move_x_timer * 137;
		const float camera_x_pos = -200 + camera_x_timer * 137;

		sprites[0]->transform->local_pos.x = move_x_pos;
		sprites[1]->transform->local_pos.x = camera_x_pos;

		if (move_x_timer == 1 && camera_x_timer == 1) {
			tutrial_flag++;
			tutrial_timer = 0;

			tutrial_move_check_animation_timer = 0;
			tutrial_camera_check_animation_timer = 0;
			tutrial_move_state_timer = 0;
			tutrial_camera_state_timer = 0;
		}

	}

	// move,cameraのtutrialを行う
	if (tutrial_flag == 2) {
		if (input_changer->dir.norm() != 0)        tutrial_move_state_timer += time->deltaTime(); //移動キーを押されたらtimerを進める
		if (input_changer->cursol_move.norm() != 0)tutrial_camera_state_timer += time->deltaTime(); //cameraを回転させてらtimerを進める

		// 2秒以上動いたらチェックマークをつける
		if (tutrial_move_state_timer >= 2) {
			// scaleを適当な二次関数でeasing
			tutrial_move_check_animation_timer += time->deltaTime() * 4;
			tutrial_move_check_animation_timer = ALClamp(tutrial_move_check_animation_timer, 0, 1);

			float t = tutrial_move_check_animation_timer - 0.35f;
			float q_func = -0.7f * t * t + 1.3f;

			for (auto& child : *sprites[0]->children()) {
				child->is_active = true; //見えるようにする
				child->transform->local_scale = check_base_scale * q_func;
			}
		}

		// 1秒以上cameraを動かしたらチェックマークをつける
		if (tutrial_camera_state_timer >= 1) {
			// scaleを適当な二次関数でeasing
			tutrial_camera_check_animation_timer += time->deltaTime() * 4;
			tutrial_camera_check_animation_timer = ALClamp(tutrial_camera_check_animation_timer, 0, 1);

			float t = tutrial_camera_check_animation_timer - 0.35f;
			float q_func = -0.7f * t * t + 1.3f;

			for (auto& child : *sprites[1]->children()) {
				child->is_active = true; //見えるようにする
				child->transform->local_scale = check_base_scale * q_func;
			}
		}
		// どちらもチェックマークがついたら次のflagへ
		if (tutrial_move_check_animation_timer == 1 && tutrial_camera_check_animation_timer == 1) {
			tutrial_flag++;
			tutrial_timer = 0;
		}

	}
	// 1秒待つ
	if (tutrial_flag == 3) {
		for (auto& child : *sprites[0]->children())child->is_active = true;
		for (auto& child : *sprites[1]->children())child->is_active = true;

		if (tutrial_timer > 1) {
			tutrial_flag++;
			tutrial_timer = 0;
		}

	}
	// 左に引っ込む
	if (tutrial_flag == 4) {
		for (auto& child : *sprites[0]->children())child->is_active = true;
		for (auto& child : *sprites[1]->children())child->is_active = true;

		const float move_x_timer = ALClamp(1 - tutrial_timer * 5, 0, 1);
		const float camera_x_timer = ALClamp(1 - (tutrial_timer * 5 - 0.2f), 0, 1);
		const float move_x_pos = -200 + move_x_timer * 137;
		const float camera_x_pos = -200 + camera_x_timer * 137;

		sprites[0]->transform->local_pos.x = move_x_pos;
		sprites[1]->transform->local_pos.x = camera_x_pos;

		if (move_x_timer == 0 && camera_x_timer == 0) {
			tutrial_flag++;
			tutrial_timer = 0;
		}
	}


}

void Tutrial_stage01_move_camera_catch::tutrial_hand_stretch() {

	go_tutrial_hand->transform->local_pos.y = 80;
	if(tutrial_flag > 5) go_tutrial_hand->is_active = true;

	//::::::::

	// 5秒待つ
	if (tutrial_flag == 5) {

		if (tutrial_timer > 5) {
			tutrial_flag++;
			tutrial_timer = 0;
		}
	}

	// 左から出てくる
	if (tutrial_flag == 6) {
		const float move_x_timer = ALClamp(tutrial_timer * 5, 0, 1);
		const float move_x_pos = -200 + move_x_timer * 137;

		go_tutrial_hand->transform->local_pos.x = move_x_pos;

		if (move_x_timer == 1) {
			tutrial_flag++;
			tutrial_timer = 0;

			tutrial_move_check_animation_timer = 0; //timerを使いまわす
			tutrial_move_state_timer = 0;
		}

	}

	// move,cameraのtutrialを行う
	if (tutrial_flag == 7) {
		if (input_changer->is_Larm_state || input_changer->is_Rarm_state) tutrial_move_state_timer += time->deltaTime(); //移動キーを押されたらtimerを進める

		// 3秒以上動いたらチェックマークをつける
		if (tutrial_move_state_timer >= 3) {
			// scaleを適当な二次関数でeasing
			tutrial_move_check_animation_timer += time->deltaTime() * 4;
			tutrial_move_check_animation_timer = ALClamp(tutrial_move_check_animation_timer, 0, 1);

			float t = tutrial_move_check_animation_timer - 0.35f;
			float q_func = -0.7f * t * t + 1.3f;

			for (auto& child : *go_tutrial_hand->children()) {
				child->is_active = true; //見えるようにする
				child->transform->local_scale = check_base_scale * q_func;
			}
		}

		// どちらもチェックマークがついたら次のflagへ
		if (tutrial_move_check_animation_timer == 1) {
			tutrial_flag++;
			tutrial_timer = 0;
		}

	}
	// 1秒待つ
	if (tutrial_flag == 8) {
		for (auto& child : *go_tutrial_hand->children())child->is_active = true;

		if (tutrial_timer > 1) {
			tutrial_flag++;
			tutrial_timer = 0;
		}

	}
	// 左に引っ込む
	if (tutrial_flag == 9) {
		for (auto& child : *go_tutrial_hand->children())child->is_active = true;

		const float move_x_timer = ALClamp(1 - tutrial_timer * 5, 0, 1);
		const float move_x_pos = -200 + move_x_timer * 137;

		go_tutrial_hand->transform->local_pos.x = move_x_pos;

		if (move_x_timer == 0) {
			tutrial_flag++;
			tutrial_timer = 0;
		}
	}

}

void Tutrial_stage01_move_camera_catch::tutrial_hand_catch() {

	go_tutrial_catch_object->transform->local_pos.y = 80;
	if (tutrial_flag > 10) go_tutrial_catch_object->is_active = true;

	// 0秒待つ
	if (tutrial_flag == 10) {
		player->get_stage_manager_ptr()->get_current_stage()->summon_block_01();

		if (tutrial_timer > 0) {
			tutrial_flag++;
			tutrial_timer = 0;
		}
	}

	// 左から出てくる
	if (tutrial_flag == 11) {
		const float move_x_timer = ALClamp(tutrial_timer * 5, 0, 1);
		const float move_x_pos = -200 + move_x_timer * 137;

		go_tutrial_catch_object->transform->local_pos.x = move_x_pos;

		if (move_x_timer == 1) {
			tutrial_flag++;
			tutrial_timer = 0;

			tutrial_move_check_animation_timer = 0; //timerを使いまわす
			tutrial_move_state_timer = 0;
		}

	}

	// move,cameraのtutrialを行う
	if (tutrial_flag == 12) {
		if (player->is_chatch_objct(0) || player->is_chatch_objct(1)) tutrial_move_state_timer += time->deltaTime(); //移動キーを押されたらtimerを進める

		// 3秒以上つかんだらチェックマークをつける
		if (tutrial_move_state_timer >= 3) {
			// scaleを適当な二次関数でeasing
			tutrial_move_check_animation_timer += time->deltaTime() * 4;
			tutrial_move_check_animation_timer = ALClamp(tutrial_move_check_animation_timer, 0, 1);

			float t = tutrial_move_check_animation_timer - 0.35f;
			float q_func = -0.7f * t * t + 1.3f;

			for (auto& child : *go_tutrial_catch_object->children()) {
				child->is_active = true; //見えるようにする
				child->transform->local_scale = check_base_scale * q_func;
			}
		}

		// どちらもチェックマークがついたら次のflagへ
		if (tutrial_move_check_animation_timer == 1) {
			tutrial_flag++;
			tutrial_timer = 0;
		}

	}
	// 1秒待つ
	if (tutrial_flag == 13) {
		for (auto& child : *go_tutrial_catch_object->children())child->is_active = true;

		if (tutrial_timer > 1) {
			tutrial_flag++;
			tutrial_timer = 0;
		}

	}
	// 左に引っ込む
	if (tutrial_flag == 14) {
		for (auto& child : *go_tutrial_catch_object->children())child->is_active = true;

		const float move_x_timer = ALClamp(1 - tutrial_timer * 5, 0, 1);
		const float move_x_pos = -200 + move_x_timer * 137;

		go_tutrial_catch_object->transform->local_pos.x = move_x_pos;

		if (move_x_timer == 0) {
			tutrial_flag++;
			tutrial_timer = 0;
		}
	}
}