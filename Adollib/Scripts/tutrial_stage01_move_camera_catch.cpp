
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

	// material�̍쐬
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

		mat_tutrial_jump = Material_manager::create_material("mat_tutrial_jump");
		mat_tutrial_jump->get_texture()->Load(L"./DefaultTexture/tutrial/tutrial_A_jump.png");
		mat_tutrial_jump->BS_state = State_manager::BStypes::BS_ALPHA;
		mat_tutrial_jump->Load_VS("./DefaultShader/ui_vs.cso");
		mat_tutrial_jump->Load_PS("./DefaultShader/ui_ps.cso");
		mat_tutrial_jump->is_render_shadow = false;

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

		mat_tutrial_hand_dir = Material_manager::create_material("mat_tutrial_hand_dir");
		mat_tutrial_hand_dir->get_texture()->Load(L"./DefaultTexture/tutrial/tutrial_hand_dir.png");
		mat_tutrial_hand_dir->BS_state = State_manager::BStypes::BS_ALPHA;
		mat_tutrial_hand_dir->Load_VS("./DefaultShader/ui_vs.cso");
		mat_tutrial_hand_dir->Load_PS("./DefaultShader/ui_ps.cso");
		mat_tutrial_hand_dir->is_render_shadow = false;
	}

	// gameobject�̍쐬
	{
		Vector3 local_check_pos = Vector3(5.15f, 2.9f, 1);
		Vector3 local_check_pos_y2 = Vector3(5.15f, 6.3f, 1);
		check_base_scale = Vector3(0.21f, 1.5f, 1);
		check_base_scale_y2 = Vector3(0.21f, 1.5f * 0.5f, 1);

		{
			go_tutrial_move = Gameobject_manager::createPlane("go_tutrial_move", Scenelist::scene_player);
			auto renderer = go_tutrial_move->addComponent<UI_renderer>();
			renderer->set_material(mat_tutrial_move);
			go_tutrial_move->transform->local_scale = Vector3(7, 1, 1);

			auto check = Gameobject_manager::createPlane("check", Scenelist::scene_player);
			auto check_renderer = check->addComponent<UI_renderer>();
			check_renderer->set_material(mat_tutrial_check);
			check_renderer->depth = 1;
			check->transform->local_scale = check_base_scale;
			check->transform->local_pos = local_check_pos;
			go_tutrial_move->add_child(check);

		}
		{
			go_tutrial_camera = Gameobject_manager::createPlane("go_tutrial_camera", Scenelist::scene_player);
			auto renderer = go_tutrial_camera->addComponent<UI_renderer>();
			renderer->set_material(mat_tutrial_camera);
			go_tutrial_camera->transform->local_scale = Vector3(7, 1, 1);

			auto check = Gameobject_manager::createPlane("check", Scenelist::scene_player);
			auto check_renderer = check->addComponent<UI_renderer>();
			check_renderer->set_material(mat_tutrial_check);
			check_renderer->depth = 1;
			check->transform->local_scale = check_base_scale;
			check->transform->local_pos = local_check_pos;
			go_tutrial_camera->add_child(check);
		}
		{
			go_tutrial_jump= Gameobject_manager::createPlane("go_tutrial_jump", Scenelist::scene_player);
			auto renderer = go_tutrial_jump->addComponent<UI_renderer>();
			renderer->set_material(mat_tutrial_jump);
			go_tutrial_jump->transform->local_scale = Vector3(7, 1, 1);

			auto check = Gameobject_manager::createPlane("check", Scenelist::scene_player);
			auto check_renderer = check->addComponent<UI_renderer>();
			check_renderer->set_material(mat_tutrial_check);
			check_renderer->depth = 1;
			check->transform->local_scale = check_base_scale;
			check->transform->local_pos = local_check_pos;
			go_tutrial_jump->add_child(check);
		}
		{
			go_tutrial_hand = Gameobject_manager::createPlane("go_tutrial_hand", Scenelist::scene_player);
			auto renderer = go_tutrial_hand->addComponent<UI_renderer>();
			renderer->set_material(mat_tutrial_hand);
			go_tutrial_hand->transform->local_scale = Vector3(7, 1, 1);

			auto check = Gameobject_manager::createPlane("check", Scenelist::scene_player);
			auto check_renderer = check->addComponent<UI_renderer>();
			check_renderer->set_material(mat_tutrial_check);
			check_renderer->depth = 1;
			check->transform->local_scale = check_base_scale;
			check->transform->local_pos = local_check_pos;
			go_tutrial_hand->add_child(check);
		}
		{
			go_tutrial_catch_object = Gameobject_manager::createPlane("go_tutrial_catch_object", Scenelist::scene_player);
			auto renderer = go_tutrial_catch_object->addComponent<UI_renderer>();
			renderer->set_material(mat_tutrial_catch_object);
			go_tutrial_catch_object->transform->local_scale = Vector3(7, 1, 1);

			auto check = Gameobject_manager::createPlane("check", Scenelist::scene_player);
			auto check_renderer = check->addComponent<UI_renderer>();
			check_renderer->set_material(mat_tutrial_check);
			check_renderer->depth = 1;
			check->transform->local_scale = check_base_scale;
			check->transform->local_pos = local_check_pos;
			go_tutrial_catch_object->add_child(check);
		}
		{
			go_tutrial_hand_dir = Gameobject_manager::createPlane("go_tutrial_hand_dir", Scenelist::scene_player);
			auto renderer = go_tutrial_hand_dir->addComponent<UI_renderer>();
			renderer->set_material(mat_tutrial_hand_dir);
			go_tutrial_hand_dir->transform->local_scale = Vector3(7, 2, 1);

			auto check = Gameobject_manager::createPlane("check", Scenelist::scene_player);
			auto check_renderer = check->addComponent<UI_renderer>();
			check_renderer->set_material(mat_tutrial_check);
			check_renderer->depth = 1;
			check->transform->local_scale = check_base_scale_y2;
			check->transform->local_pos = local_check_pos_y2;
			go_tutrial_hand_dir->add_child(check);
		}

	}

	tutrial_flag = -1;
}

void Tutrial_stage01_move_camera_catch::update() {

	tutrial_timer += time->deltaTime();

	// �Ƃ肠������\���� �\������ۂ͖��t���[��true�ɂ��Ă�ꏊ������̂Ŗ��Ȃ�
	go_tutrial_move->is_active = false;
	go_tutrial_camera->is_active = false;
	go_tutrial_jump->is_active = false;
	go_tutrial_hand->is_active = false;
	go_tutrial_catch_object->is_active = false;
	go_tutrial_hand_dir->is_active = false;
	for (auto& child : *go_tutrial_move->children())child->is_active = false;
	for (auto& child : *go_tutrial_camera->children())child->is_active = false;
	for (auto& child : *go_tutrial_jump->children())child->is_active = false;
	for (auto& child : *go_tutrial_hand->children())child->is_active = false;
	for (auto& child : *go_tutrial_catch_object->children())child->is_active = false;
	for (auto& child : *go_tutrial_hand_dir->children())child->is_active = false;
	//return;

	// move��camera��tutrial
	// set_tutrial_move_and_camera_flag�͏����l-1 ������0�ɂȂ�A�ω����Ă���
	if (tutrial_flag != -1) {
		tutrial_move_and_camera();
		tutrial_jump();
		tutrial_hand_stretch();
		tutrial_hand_catch();
		tutrial_hand_dir();
	}

	// stage��������΂Ƃ肠���������� & return
	if (player->get_stage_manager_ptr() == nullptr) {
		tutrial_flag = -1;
		return;
	};

	// stage��change���ꂽ��flag��������
	if (player->get_stage_manager_ptr()->is_changed_stage == true)tutrial_flag = -1;

	// set_tutrial_move_and_camera_flag���܂��Z�b�g����Ă��Ȃ� &&
	// ���݂�stage��stage_1�ł��� &&
	// stage���J�n�����5�b������
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

	// ������o�Ă���
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

	// move,camera��tutrial���s��
	if (tutrial_flag == 2) {
		if (input_changer->dir.norm() != 0)        tutrial_move_state_timer += time->deltaTime(); //�ړ��L�[�������ꂽ��timer��i�߂�
		if (input_changer->cursol_move.norm() != 0)tutrial_camera_state_timer += time->deltaTime(); //camera����]�����Ă�timer��i�߂�

		// 2�b�ȏ㓮������`�F�b�N�}�[�N������
		if (tutrial_move_state_timer >= 2) {
			// scale��K���ȓ񎟊֐���easing
			tutrial_move_check_animation_timer += time->deltaTime() * 4;
			tutrial_move_check_animation_timer = ALClamp(tutrial_move_check_animation_timer, 0, 1);

			float t = tutrial_move_check_animation_timer - 0.35f;
			float q_func = -0.7f * t * t + 1.3f;

			for (auto& child : *sprites[0]->children()) {
				child->is_active = true; //������悤�ɂ���
				child->transform->local_scale = check_base_scale * q_func;
			}
		}

		// 1�b�ȏ�camera�𓮂�������`�F�b�N�}�[�N������
		if (tutrial_camera_state_timer >= 0.8f) {
			// scale��K���ȓ񎟊֐���easing
			tutrial_camera_check_animation_timer += time->deltaTime() * 4;
			tutrial_camera_check_animation_timer = ALClamp(tutrial_camera_check_animation_timer, 0, 1);

			float t = tutrial_camera_check_animation_timer - 0.35f;
			float q_func = -0.7f * t * t + 1.3f;

			for (auto& child : *sprites[1]->children()) {
				child->is_active = true; //������悤�ɂ���
				child->transform->local_scale = check_base_scale * q_func;
			}
		}
		// �ǂ�����`�F�b�N�}�[�N�������玟��flag��
		if (tutrial_move_check_animation_timer == 1 && tutrial_camera_check_animation_timer == 1) {
			tutrial_flag++;
			tutrial_timer = 0;
		}

	}
	// 1�b�҂�
	if (tutrial_flag == 3) {
		for (auto& child : *sprites[0]->children())child->is_active = true;
		for (auto& child : *sprites[1]->children())child->is_active = true;

		if (tutrial_timer > 1) {
			tutrial_flag++;
			tutrial_timer = 0;
		}

	}
	// ���Ɉ�������
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

void Tutrial_stage01_move_camera_catch::tutrial_jump() {

	go_tutrial_jump->transform->local_pos.y = 80;
	if (tutrial_flag > 5) go_tutrial_jump->is_active = true;

	//::::::::

	// 0�b�҂�
	if (tutrial_flag == 5) {

		if (tutrial_timer > 0) {
			tutrial_flag++;
			tutrial_timer = 0;
		}
	}

	// ������o�Ă���
	if (tutrial_flag == 6) {
		const float move_x_timer = ALClamp(tutrial_timer * 5, 0, 1);
		const float move_x_pos = -200 + move_x_timer * 137;

		go_tutrial_jump->transform->local_pos.x = move_x_pos;

		if (move_x_timer == 1) {
			tutrial_flag++;
			tutrial_timer = 0;

			tutrial_move_check_animation_timer = 0; //timer���g���܂킷
			tutrial_move_state_timer = 0;
		}

	}

	// move,camera��tutrial���s��
	if (tutrial_flag == 7) {
		if (input_changer->is_jump_trigger) tutrial_move_state_timer += 1; //�ړ��L�[�������ꂽ��timer��i�߂�

		// 1��jump������`�F�b�N�}�[�N��t����
		if (tutrial_move_state_timer > 0) {
			// scale��K���ȓ񎟊֐���easing
			tutrial_move_check_animation_timer += time->deltaTime() * 4;
			tutrial_move_check_animation_timer = ALClamp(tutrial_move_check_animation_timer, 0, 1);

			float t = tutrial_move_check_animation_timer - 0.35f;
			float q_func = -0.7f * t * t + 1.3f;

			for (auto& child : *go_tutrial_jump->children()) {
				child->is_active = true; //������悤�ɂ���
				child->transform->local_scale = check_base_scale * q_func;
			}
		}

		// �ǂ�����`�F�b�N�}�[�N�������玟��flag��
		if (tutrial_move_check_animation_timer == 1) {
			tutrial_flag++;
			tutrial_timer = 0;
		}

	}
	// 1�b�҂�
	if (tutrial_flag == 8) {
		for (auto& child : *go_tutrial_jump->children())child->is_active = true;

		if (tutrial_timer > 1) {
			tutrial_flag++;
			tutrial_timer = 0;
		}

	}
	// ���Ɉ�������
	if (tutrial_flag == 9) {
		for (auto& child : *go_tutrial_jump->children())child->is_active = true;

		const float move_x_timer = ALClamp(1 - tutrial_timer * 5, 0, 1);
		const float move_x_pos = -200 + move_x_timer * 137;

		go_tutrial_jump->transform->local_pos.x = move_x_pos;

		if (move_x_timer == 0) {
			tutrial_flag++;
			tutrial_timer = 0;
		}
	}

}

void Tutrial_stage01_move_camera_catch::tutrial_hand_stretch() {

	go_tutrial_hand->transform->local_pos.y = 80;
	if(tutrial_flag > 10) go_tutrial_hand->is_active = true;

	//::::::::

	// 3�b�҂�
	if (tutrial_flag == 10) {

		if (tutrial_timer > 3) {
			tutrial_flag++;
			tutrial_timer = 0;
		}
	}

	// ������o�Ă���
	if (tutrial_flag == 11) {
		const float move_x_timer = ALClamp(tutrial_timer * 5, 0, 1);
		const float move_x_pos = -200 + move_x_timer * 137;

		go_tutrial_hand->transform->local_pos.x = move_x_pos;

		if (move_x_timer == 1) {
			tutrial_flag++;
			tutrial_timer = 0;

			tutrial_move_check_animation_timer = 0; //timer���g���܂킷
			tutrial_move_state_timer = 0;
		}

	}

	// move,camera��tutrial���s��
	if (tutrial_flag == 12) {
		if (input_changer->is_Larm_state || input_changer->is_Rarm_state) tutrial_move_state_timer += time->deltaTime(); //�ړ��L�[�������ꂽ��timer��i�߂�

		// 2�b�ȏ㓮������`�F�b�N�}�[�N������
		if (tutrial_move_state_timer >= 2) {
			// scale��K���ȓ񎟊֐���easing
			tutrial_move_check_animation_timer += time->deltaTime() * 4;
			tutrial_move_check_animation_timer = ALClamp(tutrial_move_check_animation_timer, 0, 1);

			float t = tutrial_move_check_animation_timer - 0.35f;
			float q_func = -0.7f * t * t + 1.3f;

			for (auto& child : *go_tutrial_hand->children()) {
				child->is_active = true; //������悤�ɂ���
				child->transform->local_scale = check_base_scale * q_func;
			}
		}

		// �ǂ�����`�F�b�N�}�[�N�������玟��flag��
		if (tutrial_move_check_animation_timer == 1) {
			tutrial_flag++;
			tutrial_timer = 0;
		}

	}
	// 1�b�҂�
	if (tutrial_flag == 13) {
		for (auto& child : *go_tutrial_hand->children())child->is_active = true;

		if (tutrial_timer > 1) {
			tutrial_flag++;
			tutrial_timer = 0;
		}

	}
	// ���Ɉ�������
	if (tutrial_flag == 14) {
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
	if (tutrial_flag > 15) go_tutrial_catch_object->is_active = true;

	// 0�b�҂�
	if (tutrial_flag == 15) {
		player->get_stage_manager_ptr()->get_current_stage()->summon_block_01();

		if (tutrial_timer > 0) {
			tutrial_flag++;
			tutrial_timer = 0;
		}
	}

	// ������o�Ă���
	if (tutrial_flag == 16) {
		const float move_x_timer = ALClamp(tutrial_timer * 5, 0, 1);
		const float move_x_pos = -200 + move_x_timer * 137;

		go_tutrial_catch_object->transform->local_pos.x = move_x_pos;

		if (move_x_timer == 1) {
			tutrial_flag++;
			tutrial_timer = 0;

			tutrial_move_check_animation_timer = 0; //timer���g���܂킷
			tutrial_move_state_timer = 0;
		}

	}

	// move,camera��tutrial���s��
	if (tutrial_flag == 17) {
		if (player->is_chatch_objct(0) || player->is_chatch_objct(1)) tutrial_move_state_timer += time->deltaTime(); //�ړ��L�[�������ꂽ��timer��i�߂�

		// 3�b�ȏ���񂾂�`�F�b�N�}�[�N������
		if (tutrial_move_state_timer >= 3) {
			// scale��K���ȓ񎟊֐���easing
			tutrial_move_check_animation_timer += time->deltaTime() * 4;
			tutrial_move_check_animation_timer = ALClamp(tutrial_move_check_animation_timer, 0, 1);

			float t = tutrial_move_check_animation_timer - 0.35f;
			float q_func = -0.7f * t * t + 1.3f;

			for (auto& child : *go_tutrial_catch_object->children()) {
				child->is_active = true; //������悤�ɂ���
				child->transform->local_scale = check_base_scale * q_func;
			}
		}

		// �ǂ�����`�F�b�N�}�[�N�������玟��flag��
		if (tutrial_move_check_animation_timer == 1) {
			tutrial_flag++;
			tutrial_timer = 0;
		}

	}
	// 1�b�҂�
	if (tutrial_flag == 18) {
		for (auto& child : *go_tutrial_catch_object->children())child->is_active = true;

		if (tutrial_timer > 1) {
			tutrial_flag++;
			tutrial_timer = 0;
		}

	}
	// ���Ɉ�������
	if (tutrial_flag == 19) {
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

void Tutrial_stage01_move_camera_catch::tutrial_hand_dir() {

	go_tutrial_hand_dir->transform->local_pos.y = 70;
	if (tutrial_flag > 20) go_tutrial_hand_dir->is_active = true;

	// 3�b�҂�
	if (tutrial_flag == 20) {
		if (tutrial_timer > 3) {
			tutrial_flag++;
			tutrial_timer = 0;
		}
	}

	// ������o�Ă���
	if (tutrial_flag == 21) {
		const float move_x_timer = ALClamp(tutrial_timer * 5, 0, 1);
		const float move_x_pos = -200 + move_x_timer * 137;

		go_tutrial_hand_dir->transform->local_pos.x = move_x_pos;

		if (move_x_timer == 1) {
			tutrial_flag++;
			tutrial_timer = 0;

			tutrial_move_check_animation_timer = 0; //timer���g���܂킷
			tutrial_move_state_timer = 0;
		}

	}

	// move,camera��tutrial���s��
	if (tutrial_flag == 22) {
		if (
			(input_changer->is_Larm_state || input_changer->is_Rarm_state) &&
			fabsf(input_changer->cursol_move.x) / time->deltaTime() > 1
			) tutrial_move_state_timer += time->deltaTime(); //�ړ��L�[�������ꂽ��timer��i�߂�

		// 3�b�ȏ���񂾂�`�F�b�N�}�[�N������
		if (tutrial_move_state_timer >= 2) {
			// scale��K���ȓ񎟊֐���easing
			tutrial_move_check_animation_timer += time->deltaTime() * 4;
			tutrial_move_check_animation_timer = ALClamp(tutrial_move_check_animation_timer, 0, 1);

			float t = tutrial_move_check_animation_timer - 0.35f;
			float q_func = -0.7f * t * t + 1.3f;

			for (auto& child : *go_tutrial_hand_dir->children()) {
				child->is_active = true; //������悤�ɂ���
				child->transform->local_scale = check_base_scale_y2 * q_func;
			}
		}

		// �ǂ�����`�F�b�N�}�[�N�������玟��flag��
		if (tutrial_move_check_animation_timer == 1) {
			tutrial_flag++;
			tutrial_timer = 0;
		}

	}
	// 1�b�҂�
	if (tutrial_flag == 23) {
		for (auto& child : *go_tutrial_hand_dir->children())child->is_active = true;

		if (tutrial_timer > 1) {
			tutrial_flag++;
			tutrial_timer = 0;
		}

	}
	// ���Ɉ�������
	if (tutrial_flag == 24) {
		for (auto& child : *go_tutrial_hand_dir->children())child->is_active = true;

		const float move_x_timer = ALClamp(1 - tutrial_timer * 5, 0, 1);
		const float move_x_pos = -200 + move_x_timer * 137;

		go_tutrial_hand_dir->transform->local_pos.x = move_x_pos;

		if (move_x_timer == 0) {
			tutrial_flag++;
			tutrial_timer = 0;
		}
	}
}