#include "player.h"

#include "../Adollib/Scripts/Main/Adollib.h"
#include "../Adollib/Scripts/Object/gameobject_manager.h"

#include "../Adollib/Scripts/Physics/ray.h"

#include "stage_manager.h"

namespace Adollib
{
	// 所属するシーンの初期化時に一度だけ呼ばれる
	void Player::awake() {
		rotate = quaternion_from_euler(0, 180, 1);

		auto GO = Gameobject_manager::create("debugGO");
	}

	void Player::start()
	{
		//respown_timer = 1;

		camera = Gameobject_manager::find("camera")->transform;
		stage_manager = Gameobject_manager::find("Stage_manager")->findComponent<Stage_manager>();

		//auto R = Head->findComponent<Mesh_renderer>();
		////auto C = Gameobject_manager::find("camera")->findComponent<Camera_component>();
		////R->set_texture(C->get_color_texture());
		//R->load_texture(L"./DefaultModel/demo.png");

		head_rot_max_speed = 20;
		head_rot_max_pow = 1000;
		head_rot_pow = 1000;

		//waist_move_max_speed = 10;
		//waist_move_pow = 1000;
		waist_move_max_speed = 100000;
		waist_move_pow = 500;

		waist_rot_max_speed = 3;
		waist_rot_max_pow = 10000;
		waist_rot_pow = 10000;

		body_rot_max_speed = 3;
		body_rot_max_pow = 10000;
		body_rot_pow = 10000;

		leg_rot_max_speed = 5;
		leg_rot_max_pow = 300;
		leg_rot_pow = 400;

		hand_rot_max_speed = 8;
		hand_rot_max_pow = 300;
		hand_rot_pow = 300;

		hand_camera_max_rot = ToRadian(80);
		hand_camera_rot_pow = 3;
		hand_camera_rot_center = ToRadian(43);

		jump_power = 25;
		turn_speed = 1.5f;


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

	}

	// 毎フレーム呼ばれる更新処理
	void Player::update()
	{
		// Update_pnground
		{
			if (input->getKeyTrigger(Key::P)) {
				bool next = Human_colliders[0]->physics_data.is_moveable;
				next = !next;
				for (int i = 0; i < Human_collider_size; i++) {
					Human_colliders[i]->physics_data.is_moveable = next;
				}
			}
		}

		// respown処理
		//if (check_respown() == true)return;

		update_onground();

		// 手を伸ばす
		reach_out_hands();

		// 物をつかむ
		catch_things();

		// rayを飛ばして腰を立たせる
		push_waist_for_stand();

		// 移動
		linear_move();

		// 回転
		angula_move();

		// 移動方向を計算
		accume_move_dir();

		// 立つように力を加える
		add_pow_for_stand();

		// 足を動かす
		move_legs();

		// jumpさせる
		make_jump();

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

		ImGui::DragFloat("jump_power", &jump_power, 0.1f);
		ImGui::DragFloat("turn_speed", &turn_speed, 0.1f);

	};

	// このスクリプトがアタッチされているGOのactiveSelfがtrueになった時呼ばれる
	void Player::onEnable()
	{

	}

	// このスクリプトがアタッチされているGOのactiveSelfがfalseになった時呼ばれる
	void Player::onDisable()
	{

	}

}
