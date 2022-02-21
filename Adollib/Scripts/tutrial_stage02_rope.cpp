
#include "tutrial_stage02_rope.h"

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

void Tutrial_stage02_rope::awake() {

	// material�̍쐬
	{
		mat_tutrial_check = Material_manager::create_material("mat_tutrial_check");
		mat_tutrial_check->get_texture()->Load(L"./DefaultTexture/tutrial/tutrial_check.png");
		mat_tutrial_check->BS_state = State_manager::BStypes::BS_ALPHA;
		mat_tutrial_check->Load_VS("./DefaultShader/ui_vs.cso");
		mat_tutrial_check->Load_PS("./DefaultShader/ui_ps.cso");
		mat_tutrial_check->is_render_shadow = false;

		mat_tutrial_aim = Material_manager::create_material("mat_tutrial_aim");
		mat_tutrial_aim->get_texture()->Load(L"./DefaultTexture/tutrial/tutrial_LBhold_aim.png");
		mat_tutrial_aim->BS_state = State_manager::BStypes::BS_ALPHA;
		mat_tutrial_aim->Load_VS("./DefaultShader/ui_vs.cso");
		mat_tutrial_aim->Load_PS("./DefaultShader/ui_ps.cso");
		mat_tutrial_aim->is_render_shadow = false;

		mat_tutrial_shot = Material_manager::create_material("mat_tutrial_shot");
		mat_tutrial_shot->get_texture()->Load(L"./DefaultTexture/tutrial/tutrial_LBrelease_shot.png");
		mat_tutrial_shot->BS_state = State_manager::BStypes::BS_ALPHA;
		mat_tutrial_shot->Load_VS("./DefaultShader/ui_vs.cso");
		mat_tutrial_shot->Load_PS("./DefaultShader/ui_ps.cso");
		mat_tutrial_shot->is_render_shadow = false;

		mat_tutrial_shrink = Material_manager::create_material("mat_tutrial_shrink");
		mat_tutrial_shrink->get_texture()->Load(L"./DefaultTexture/tutrial/tutrial_RBhold_shrimk.png");
		mat_tutrial_shrink->BS_state = State_manager::BStypes::BS_ALPHA;
		mat_tutrial_shrink->Load_VS("./DefaultShader/ui_vs.cso");
		mat_tutrial_shrink->Load_PS("./DefaultShader/ui_ps.cso");
		mat_tutrial_shrink->is_render_shadow = false;

		mat_tutrial_cut = Material_manager::create_material("mat_tutrial_cut");
		mat_tutrial_cut->get_texture()->Load(L"./DefaultTexture/tutrial/tutrial_B_cut.png");
		mat_tutrial_cut->BS_state = State_manager::BStypes::BS_ALPHA;
		mat_tutrial_cut->Load_VS("./DefaultShader/ui_vs.cso");
		mat_tutrial_cut->Load_PS("./DefaultShader/ui_ps.cso");
		mat_tutrial_cut->is_render_shadow = false;
	}

	// gameobject�̍쐬
	{
		Vector3 local_check_pos = Vector3(5.15f, 2.9f, 1);
		check_base_scale = Vector3(0.21f, 1.5f, 1);


		{
			go_tutrial_aim = Gameobject_manager::createPlane("go_tutrial_aim", Scenelist::scene_player);
			auto renderer = go_tutrial_aim->addComponent<UI_renderer>();
			renderer->set_material(mat_tutrial_aim);
			go_tutrial_aim->transform->local_scale = Vector3(7, 1, 1);

			auto check = Gameobject_manager::createPlane("check", Scenelist::scene_player);
			auto check_renderer = check->addComponent<UI_renderer>();
			check_renderer->set_material(mat_tutrial_check);
			check_renderer->depth = 1;
			check->transform->local_scale = check_base_scale;
			check->transform->local_pos = local_check_pos;
			go_tutrial_aim->add_child(check);

		}
		{
			go_tutrial_shot = Gameobject_manager::createPlane("go_tutrial_shot", Scenelist::scene_player);
			auto renderer = go_tutrial_shot->addComponent<UI_renderer>();
			renderer->set_material(mat_tutrial_shot);
			go_tutrial_shot->transform->local_scale = Vector3(7, 1, 1);

			auto check = Gameobject_manager::createPlane("check", Scenelist::scene_player);
			auto check_renderer = check->addComponent<UI_renderer>();
			check_renderer->set_material(mat_tutrial_check);
			check_renderer->depth = 1;
			check->transform->local_scale = check_base_scale;
			check->transform->local_pos = local_check_pos;
			go_tutrial_shot->add_child(check);

		}
		{
			go_tutrial_shrink = Gameobject_manager::createPlane("go_tutrial_shrink", Scenelist::scene_player);
			auto renderer = go_tutrial_shrink->addComponent<UI_renderer>();
			renderer->set_material(mat_tutrial_shrink);
			go_tutrial_shrink->transform->local_scale = Vector3(7, 1, 1);

			auto check = Gameobject_manager::createPlane("check", Scenelist::scene_player);
			auto check_renderer = check->addComponent<UI_renderer>();
			check_renderer->set_material(mat_tutrial_check);
			check_renderer->depth = 1;
			check->transform->local_scale = check_base_scale;
			check->transform->local_pos = local_check_pos;
			go_tutrial_shrink->add_child(check);

		}
		{
			go_tutrial_cut = Gameobject_manager::createPlane("go_tutrial_cut", Scenelist::scene_player);
			auto renderer = go_tutrial_cut->addComponent<UI_renderer>();
			renderer->set_material(mat_tutrial_cut);
			go_tutrial_cut->transform->local_scale = Vector3(7, 1, 1);

			auto check = Gameobject_manager::createPlane("check", Scenelist::scene_player);
			auto check_renderer = check->addComponent<UI_renderer>();
			check_renderer->set_material(mat_tutrial_check);
			check_renderer->depth = 1;
			check->transform->local_scale = check_base_scale;
			check->transform->local_pos = local_check_pos;
			go_tutrial_cut->add_child(check);

		}

	}

	tutrial_flag = -1;
}

void Tutrial_stage02_rope::update() {

	tutrial_timer += time->deltaTime();

	// �Ƃ肠������\���� �\������ۂ͖��t���[��true�ɂ��Ă�ꏊ������̂Ŗ��Ȃ�
	go_tutrial_aim   ->is_active = false;
	go_tutrial_shot  ->is_active = false;
	go_tutrial_shrink->is_active = false;
	go_tutrial_cut   ->is_active = false;
	for (auto& child : *go_tutrial_aim   ->children())child->is_active = false;
	for (auto& child : *go_tutrial_shot  ->children())child->is_active = false;
	for (auto& child : *go_tutrial_shrink->children())child->is_active = false;
	for (auto& child : *go_tutrial_cut   ->children())child->is_active = false;
	//return;

	// move��camera��tutrial
	// set_tutrial_move_and_camera_flag�͏����l-1 ������0�ɂȂ�A�ω����Ă���
	if (tutrial_flag != -1) {
		tutrial_aim_and_shot();
		tutrial_shrink(); //1 ~ 4
		tutrial_cut(); //1 ~ 4
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
		player->get_is_shotable_rope() &&
		player->get_stage_manager_ptr()->now_stage == Stage_types::stage_2
		)
		tutrial_flag = 0;

}

void Tutrial_stage02_rope::tutrial_aim_and_shot() {

	Gameobject* sprites[2]{
		go_tutrial_aim,
		go_tutrial_shot
	};
	int flag_num = 0;

	sprites[0]->transform->local_pos.y = 80;
	sprites[1]->transform->local_pos.y = 55;

	sprites[0]->is_active = true;
	sprites[1]->is_active = true;


	if (tutrial_flag == flag_num + 0) {
		tutrial_flag++;
		tutrial_timer = 0;
	}

	// ������o�Ă���
	if (tutrial_flag == flag_num + 1) {
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
			tutrial_aim_state_timer  = 0;
			tutrial_shot_state_timer = 0;
		}

	}

	// move,camera��tutrial���s��
	if (tutrial_flag == flag_num + 2) {
		if (input_changer->is_rope_state)    tutrial_aim_state_timer  = 1; // aim���s������
		if (input_changer->is_rope_releaced) tutrial_shot_state_timer = 1; // rope�𔭎˂�����

		// aim���s������`�F�b�N�}�[�N������
		if (tutrial_aim_state_timer != 0) {
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

		// rope�𔭎˂�����`�F�b�N�}�[�N������
		if (tutrial_shot_state_timer != 0) {
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
	if (tutrial_flag == flag_num + 3) {
		for (auto& child : *sprites[0]->children())child->is_active = true;
		for (auto& child : *sprites[1]->children())child->is_active = true;

		if (tutrial_timer > 1) {
			tutrial_flag++;
			tutrial_timer = 0;
		}

	}
	// ���Ɉ�������
	if (tutrial_flag == flag_num + 4) {
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

void Tutrial_stage02_rope::tutrial_shrink() {

	auto go = go_tutrial_shrink;
	int flag_num = 5;

	go->transform->local_pos.y = 80;
	if (tutrial_flag > flag_num) go->is_active = true;

	//::::::::

	// 0�b�҂�
	if (tutrial_flag == flag_num + 0) {

		if (tutrial_timer > 0) {
			tutrial_flag++;
			tutrial_timer = 0;
		}
	}

	// ������o�Ă���
	if (tutrial_flag == flag_num + 1) {
		const float move_x_timer = ALClamp(tutrial_timer * 5, 0, 1);
		const float move_x_pos = -200 + move_x_timer * 137;

		go->transform->local_pos.x = move_x_pos;

		if (move_x_timer == 1) {
			tutrial_flag++;
			tutrial_timer = 0;

			tutrial_move_check_animation_timer = 0; //timer���g���܂킷
			tutrial_aim_state_timer = 0;
		}

	}

	// move,camera��tutrial���s��
	if (tutrial_flag == flag_num + 2) {
		if (input_changer->is_rope_shrink) tutrial_aim_state_timer += time->deltaTime(); //�ړ��L�[�������ꂽ��timer��i�߂�

		// 3�b�ȏ�k�߂���`�F�b�N�}�[�N��t����
		if (tutrial_aim_state_timer > 3) {
			// scale��K���ȓ񎟊֐���easing
			tutrial_move_check_animation_timer += time->deltaTime() * 4;
			tutrial_move_check_animation_timer = ALClamp(tutrial_move_check_animation_timer, 0, 1);

			float t = tutrial_move_check_animation_timer - 0.35f;
			float q_func = -0.7f * t * t + 1.3f;

			for (auto& child : *go->children()) {
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
	if (tutrial_flag == flag_num + 3) {
		for (auto& child : *go->children())child->is_active = true;

		if (tutrial_timer > 1) {
			tutrial_flag++;
			tutrial_timer = 0;
		}

	}
	// ���Ɉ�������
	if (tutrial_flag == flag_num + 4) {
		for (auto& child : *go->children())child->is_active = true;

		const float move_x_timer = ALClamp(1 - tutrial_timer * 5, 0, 1);
		const float move_x_pos = -200 + move_x_timer * 137;

		go->transform->local_pos.x = move_x_pos;

		if (move_x_timer == 0) {
			tutrial_flag++;
			tutrial_timer = 0;
		}
	}
}

void Tutrial_stage02_rope::tutrial_cut() {

	auto go = go_tutrial_cut;
	int flag_num = 10;

	go->transform->local_pos.y = 80;
	if (tutrial_flag > flag_num) go->is_active = true;

	//::::::::

	// 0�b�҂�
	if (tutrial_flag == flag_num + 0) {

		if (tutrial_timer > 0) {
			tutrial_flag++;
			tutrial_timer = 0;
		}
	}

	// ������o�Ă���
	if (tutrial_flag == flag_num + 1) {
		const float move_x_timer = ALClamp(tutrial_timer * 5, 0, 1);
		const float move_x_pos = -200 + move_x_timer * 137;

		go->transform->local_pos.x = move_x_pos;

		if (move_x_timer == 1) {
			tutrial_flag++;
			tutrial_timer = 0;

			tutrial_move_check_animation_timer = 0; //timer���g���܂킷
			tutrial_aim_state_timer = 0;
		}

	}

	// move,camera��tutrial���s��
	if (tutrial_flag == flag_num + 2) {
		if (input_changer->is_rope_delete) tutrial_aim_state_timer += 1; //�ړ��L�[�������ꂽ��timer��i�߂�

		// 3�b�ȏ�k�߂���`�F�b�N�}�[�N��t����
		if (tutrial_aim_state_timer != 0) {
			// scale��K���ȓ񎟊֐���easing
			tutrial_move_check_animation_timer += time->deltaTime() * 4;
			tutrial_move_check_animation_timer = ALClamp(tutrial_move_check_animation_timer, 0, 1);

			float t = tutrial_move_check_animation_timer - 0.35f;
			float q_func = -0.7f * t * t + 1.3f;

			for (auto& child : *go->children()) {
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
	if (tutrial_flag == flag_num + 3) {
		for (auto& child : *go->children())child->is_active = true;

		if (tutrial_timer > 1) {
			tutrial_flag++;
			tutrial_timer = 0;
		}

	}
	// ���Ɉ�������
	if (tutrial_flag == flag_num + 4) {
		for (auto& child : *go->children())child->is_active = true;

		const float move_x_timer = ALClamp(1 - tutrial_timer * 5, 0, 1);
		const float move_x_pos = -200 + move_x_timer * 137;

		go->transform->local_pos.x = move_x_pos;

		if (move_x_timer == 0) {
			tutrial_flag++;
			tutrial_timer = 0;
		}
	}
}
