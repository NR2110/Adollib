#include "player.h"

#include "../Adollib/Scripts/Main/Adollib.h"
#include "../Adollib/Scripts/Object/gameobject_manager.h"

#include "../Adollib/Scripts/Physics/ray.h"

static Adollib::Collider* debug_coll = nullptr;

namespace Adollib
{
	// 所属するシーンの初期化時に一度だけ呼ばれる
	void Player::awake() {
		rotate = quaternion_from_euler(0, 180, 0);

		auto Go = Gameobject_manager::createSphere();
		Go->transform->local_pos = Vector3(0, 3, 0);
		debug_coll = Go->addComponent<Collider>();
		debug_coll->add_shape<Sphere>();
		debug_coll->physics_data.inertial_mass = 1;

	}

	void Player::start()
	{
		camera = Gameobject_manager::find("camera")->transform;

		head_rot_max_speed = 20;
		head_rot_max_pow = 1000;
		head_rot_pow = 500;

		waist_move_max_speed = 15;
		waist_move_pow = 1000;

		waist_rot_max_speed = 10;
		waist_rot_max_pow = 4000;
		waist_rot_pow = 9000;

		body_rot_max_speed = 10;
		body_rot_max_pow = 2000;
		body_rot_pow = 1500;

		leg_rot_max_speed = 60;
		leg_rot_max_pow = 200;
		leg_rot_pow = 200;

		hand_rot_max_speed = 20;
		hand_rot_max_pow = 50;
		hand_rot_pow = 15;

		hand_camera_max_rot = ToRadian(80);
		hand_camera_rot_pow = 3;
		hand_camera_rot_center = ToRadian(30);

		jump_power = 30;
		turn_speed = 2;

		{
			auto GO = Gameobject_manager::create("waist_sphere_stand");
			check_standable_collider = GO->addComponent<Collider>();
			auto shape = check_standable_collider->add_shape<Sphere>();

			Waist->add_child(GO);
			GO->transform->local_pos = Vector3(0, -3.0f, 0);
			GO->transform->local_scale = Vector3(0.5f) / Waist->transform->local_scale;

			check_standable_collider->physics_data.is_moveable = false;
			check_standable_collider->physics_data.is_hitable = false;

		}
		{
			onground_collider_GO = Gameobject_manager::create("check_onglound_go");
			onground_collider = onground_collider_GO->addComponent<Collider>();
			auto shape = onground_collider->add_shape<Sphere>();

			Waist->add_child(onground_collider_GO);
			onground_collider_GO->transform->local_scale = Vector3(0.4f) / Waist->transform->local_scale;

			onground_collider->physics_data.is_moveable = false;
			onground_collider->physics_data.is_hitable = false;
			onground_collider->ignore_tags |= Collider_tags::Human;

		}
	}

	// 毎フレーム呼ばれる更新処理
	void Player::update()
	{
		debug_coll->add_force(Vector3(0, 30, 0));

		// Update_pnground
		{
			onground_collider_GO->transform->local_pos = waist_pillar->center;
		}

		// 手を伸ばす
		reach_out_hands();

		// 物をつかむ
		catch_things();

		// 腰を支えるwaist_pillarを調整
		tuning_waist_pillar();

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
