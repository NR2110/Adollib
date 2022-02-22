
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
#include "camera.h"
#include "stage_manager.h"
#include "stage_base.h"

using namespace Adollib;

void Tutrial_stage02_rope::create_material(std::shared_ptr<Material>& mat_ptr, const std::string& material_name, const wchar_t* texture_path) {
	mat_ptr = Material_manager::create_material(material_name + std::to_string(player->get_player_num()));
	mat_ptr->only_render_cameraGO_ptr = player->get_cameraptr()->gameobject;
	mat_ptr->get_texture()->Load(texture_path);
	mat_ptr->BS_state = State_manager::BStypes::BS_ALPHA;
	mat_ptr->Load_VS("./DefaultShader/ui_vs.cso");
	mat_ptr->Load_PS("./DefaultShader/ui_ps.cso");
	mat_ptr->is_render_shadow = false;
}
void Tutrial_stage02_rope::create_gameobject(Gameobject*& retGO, const std::string& go_name, std::shared_ptr<Material>& set_mat,
	const Vector3& go_scale, const Vector3& local_check_pos, const Vector3& local_check_scale
) {
	retGO = Gameobject_manager::createPlane(go_name, Scenelist::scene_player);
	auto renderer = retGO->addComponent<UI_renderer>();
	renderer->set_material(set_mat);
	retGO->transform->local_scale = go_scale;

	auto check = Gameobject_manager::createPlane("check", Scenelist::scene_player);
	auto check_renderer = check->addComponent<UI_renderer>();
	check_renderer->set_material(mat_tutrial_check);
	check_renderer->depth = 1;
	check->transform->local_scale = local_check_scale;
	check->transform->local_pos = local_check_pos;
	retGO->add_child(check);
}


void Tutrial_stage02_rope::awake() {

	// material�̍쐬
	{
		create_material(mat_tutrial_check, "mat_tutrial_check", L"./DefaultTexture/tutrial/tutrial_check.png");
		create_material(mat_tutrial_aim, "mat_tutrial_aim", L"./DefaultTexture/tutrial/tutrial_LBhold_aim.png");
		create_material(mat_tutrial_shot, "mat_tutrial_shot", L"./DefaultTexture/tutrial/tutrial_LBrelease_shot.png");
		create_material(mat_tutrial_shrink, "mat_tutrial_shrink", L"./DefaultTexture/tutrial/tutrial_RBhold_shrimk.png");
		create_material(mat_tutrial_cut, "mat_tutrial_cut", L"./DefaultTexture/tutrial/tutrial_B_cut.png");
	}

	// gameobject�̍쐬
	{
		Vector3 local_check_pos = Vector3(5.15f, 2.9f, 1);
		check_base_scale = Vector3(0.21f, 1.5f, 1);

		create_gameobject(go_tutrial_aim, "go_tutrial_aim", mat_tutrial_aim, Vector3(7, 1, 1), local_check_pos, check_base_scale);
		create_gameobject(go_tutrial_shot, "go_tutrial_shot", mat_tutrial_shot, Vector3(7, 1, 1), local_check_pos, check_base_scale);
		create_gameobject(go_tutrial_shrink, "go_tutrial_shrink", mat_tutrial_shrink, Vector3(7, 1, 1), local_check_pos, check_base_scale);
		create_gameobject(go_tutrial_cut, "go_tutrial_cut", mat_tutrial_cut, Vector3(7, 1, 1), local_check_pos, check_base_scale);

		gameobject->add_child(go_tutrial_aim);
		gameobject->add_child(go_tutrial_shot);
		gameobject->add_child(go_tutrial_shrink);
		gameobject->add_child(go_tutrial_cut);
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
		const float move_x_pos = tutrial_ui_x + -137 + move_x_timer * 137;
		const float camera_x_pos = tutrial_ui_x + -137 + camera_x_timer * 137;

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
		const float move_x_pos = tutrial_ui_x  - 137  + move_x_timer * 137;
		const float camera_x_pos = tutrial_ui_x  - 137  + camera_x_timer * 137;

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
		const float move_x_pos = tutrial_ui_x  - 137  + move_x_timer * 137;

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
		const float move_x_pos = tutrial_ui_x  - 137  + move_x_timer * 137;

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
		const float move_x_pos = tutrial_ui_x  - 137  + move_x_timer * 137;

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
		const float move_x_pos = tutrial_ui_x  - 137  + move_x_timer * 137;

		go->transform->local_pos.x = move_x_pos;

		if (move_x_timer == 0) {
			tutrial_flag++;
			tutrial_timer = 0;
		}
	}
}
