#include "player.h"

#include "../Adollib/Scripts/Main/Adollib.h"
#include "../Adollib/Scripts/Object/gameobject_manager.h"

#include "../Adollib/Scripts/Physics/ray.h"

#include "stage_manager.h"
#include "camera.h"
#include "input_changer.h"

namespace Adollib
{
	// ��������V�[���̏��������Ɉ�x�����Ă΂��
	void Player::awake() {
		rotate = quaternion_from_euler(0, 180, 1);

	}

	void Player::start()
	{

		camera = Gameobject_manager::find("camera")->findComponent<Camera>();
		//stage_manager = Gameobject_manager::find("Stage_manager")->findComponent<Stage_manager>();
		input_changer = gameobject->findComponent<Input_changer>();
		stage_manager = nullptr;

		head_rot_max_speed = 3;
		head_rot_max_pow = 1000;
		head_rot_pow = 1000;

		waist_move_max_speed = 100000;
		waist_move_pow = 490;

		waist_rot_max_speed = 3;
		waist_rot_max_pow = 10000;
		waist_rot_pow = 1500;

		body_rot_max_speed = 10;
		body_rot_max_pow = 10000;
		body_rot_pow = 5000;

		leg_rot_max_speed = 5;
		leg_rot_max_pow = 300;
		leg_rot_pow = 400;

		hand_rot_max_speed = 8;
		hand_rot_max_pow = 300;
		hand_rot_pow = 300;

		hand_camera_max_rot = ToRadian(85);
		hand_camera_rot_pow = 2;
		hand_camera_rot_center = ToRadian(15);

		jump_y_power = 21;
		jump_front_power = 5000;

		turn_speed = 1.5f;

		// player����ɏ���Ă��邩�Agimick�������m�ł���悤��player�̉������ɓK����collider��u��
		{
			auto check_onplayer_go = Gameobject_manager::create("check_onplayer_go");
			auto check_onplayer_coll = check_onplayer_go->addComponent<Collider>();
			auto shape = check_onplayer_coll->add_shape<Sphere>();

			check_onplayer_go->transform->local_pos = Vector3(0, -3.0f, 0);

			check_onplayer_coll->physics_data.is_moveable = false;
			check_onplayer_coll->physics_data.is_hitable = false;
			check_onplayer_coll->tag = Collider_tags::Human;

			shape->r = 1;

			Waist->add_child(check_onplayer_go);
		}
		// respown���ɐڒn������s�� player����sphere
		{
			auto check_respown_go = Gameobject_manager::create("check_respown_go");
			auto check_onplayer_coll = check_respown_go->addComponent<Collider>();
			auto shape = check_onplayer_coll->add_shape<Sphere>();

			check_respown_go->transform->local_pos = Vector3(0, 0, 0);
			check_respown_go->transform->local_scale = Vector3(1) * 2.5f;

			check_onplayer_coll->physics_data.is_moveable = false;
			check_onplayer_coll->physics_data.is_hitable = false;
			shape->r = 1;

			Waist->add_child(check_respown_go);

			this->check_onplayer_coll = check_onplayer_coll;
		}

		{
			// anchor�̏�������ۑ�
			Rhand_joint_ylength_default = Rhand_joint->anchor.posA.y;
			Lhand_joint_ylength_default = Lhand_joint->anchor.posA.y;
		}

		respown_timer = -1;
		update();

	}

	// ���t���[���Ă΂��X�V����
	void Player::update()
	{
		if (stage_manager == nullptr)return; //�Ή�����stage�������� return

		// player��drag�������l�ɂ���
		set_default_data();

		// respown����
		if (check_respown() == true)return;

		// onground�̏��̍X�V
		update_onground();

		// �̂��N�����͂����߂�
		update_gnyat_pow();

		// ���L�΂�
		reach_out_hands();

		// ��������
		catch_things();

		// ray���΂��č��𗧂�����
		push_waist_for_stand();

		// �ړ�
		linear_move();

		// ��]
		angula_move();

		// �ړ��������v�Z
		accume_move_dir();

		// ���悤�ɗ͂�������
		add_pow_for_stand();

		// ���𓮂���
		move_legs();

		// jump������
		make_jump();

		shot_rope();

	}

	void Player::Update_hierarchy() {

		ImGui::DragFloat("waist_move_max_speed", &waist_move_max_speed, 0.1f);
		ImGui::DragFloat("waist_move_pow", &waist_move_pow, 0.1f);
		ImGui::Separator();

		ImGui::DragFloat("head_rot_max_speed", &head_rot_max_speed, 0.1f);
		ImGui::DragFloat("head_rot_max_pow", &head_rot_max_pow, 0.1f);
		ImGui::DragFloat("head_rot_pow", &head_rot_pow, 0.1f);
		ImGui::Separator();

		ImGui::DragFloat("waist_rot_max_speed", &waist_rot_max_speed, 0.1f);
		ImGui::DragFloat("waist_rot_max_pow", &waist_rot_max_pow, 0.1f);
		ImGui::DragFloat("waist_rot_pow", &waist_rot_pow, 0.1f);
		ImGui::Separator();

		ImGui::DragFloat("body_rot_max_speed", &body_rot_max_speed, 0.1f);
		ImGui::DragFloat("body_rot_max_pow", &body_rot_max_pow, 0.1f);
		ImGui::DragFloat("body_rot_pow", &body_rot_pow, 0.1f);
		ImGui::Separator();

		ImGui::DragFloat("leg_rot_max_speed", &leg_rot_max_speed, 0.1f);
		ImGui::DragFloat("leg_rot_max_pow", &leg_rot_max_pow, 0.1f);
		ImGui::DragFloat("leg_rot_pow", &leg_rot_pow, 0.1f);
		ImGui::Separator();

		ImGui::DragFloat("hand_rot_max_speed", &hand_rot_max_speed, 0.1f);
		ImGui::DragFloat("hand_rot_max_pow", &hand_rot_max_pow, 0.1f);
		ImGui::DragFloat("hand_rot_pow", &hand_rot_pow, 0.1f);
		ImGui::Separator();

		ImGui::DragFloat("hand_updown_max_rot", &hand_camera_max_rot, 0.1f);
		ImGui::DragFloat("hand_updown_rot_pow", &hand_camera_rot_pow, 0.1f);
		ImGui::DragFloat("hand_updown_rot_center", &hand_camera_rot_center, 0.1f);
		ImGui::Separator();

		ImGui::DragFloat("jump_y_power", &jump_y_power, 0.1f);
		ImGui::DragFloat("jump_front_power", &jump_front_power, 0.1f);
		ImGui::DragFloat("turn_speed", &turn_speed, 0.1f);

	};

	// ���̃X�N���v�g���A�^�b�`����Ă���GO��activeSelf��true�ɂȂ������Ă΂��
	void Player::onEnable()
	{

	}

	// ���̃X�N���v�g���A�^�b�`����Ă���GO��activeSelf��false�ɂȂ������Ă΂��
	void Player::onDisable()
	{

	}

}
