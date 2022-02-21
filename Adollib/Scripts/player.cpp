#include "player.h"

#include "../Adollib/Scripts/Main/Adollib.h"
#include "../Adollib/Scripts/Object/gameobject_manager.h"
#include "../Adollib/Scripts/Physics/ray.h"
#include "../Adollib/Scripts/Renderer/renderer_base.h"
#include "../Adollib/Scripts/Renderer/material_manager.h"

#include "stage_manager.h"
#include "camera.h"
#include "input_changer.h"

namespace Adollib
{
	// 所属するシーンの初期化時に一度だけ呼ばれる
	void Player::awake() {
		rotate = quaternion_from_euler(0, 180, 1);

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

		jump_y_power = 22;
		jump_front_power = 5000;

		turn_speed = 1.5f;

		// rope発射の予測線Goの生成
		{
			auto material = Material_manager::create_material("rope_prediction");
			material->is_render_shadow = false;

			rope_hit_sphere = Gameobject_manager::createSphere("rope_hit_sphere",Scenelist::scene_player);
			rope_hit_sphere->transform->local_scale = Vector3(rope_sphere_r * 2);
			rope_hit_sphere->renderer->color = Vector4(1, 0, 0, 1);
			rope_hit_sphere->renderer->set_material(material);

			rope_hit_cylinder = Gameobject_manager::createCylinder("rope_hit_cylinder",Scenelist::scene_player);
			rope_hit_cylinder->transform->local_scale = Vector3(rope_sphere_r * 0.5f);
			rope_hit_cylinder->renderer->color = Vector4(1, 0, 0, 1);
			rope_hit_cylinder->renderer->set_material(material);

			// とりあえず描画されないところに飛ばす
			rope_hit_sphere->transform->local_pos = Vector3(10000, 10000, 10000000);
			rope_hit_cylinder->transform->local_pos = Vector3(10000, 10000, 10000000);

			gameobject->add_child(rope_hit_sphere);
			gameobject->add_child(rope_hit_cylinder);
		}

		respown_timer = -1;
	}

	void Player::start()
	{
		// playerが上に乗っているか、gimick側が感知できるようにplayerの下方向に適当なcolliderを置く
		{
			auto check_onplayer_go = Gameobject_manager::create("check_onplayer_go", Scenelist::scene_player);
			auto check_onplayer_coll = check_onplayer_go->addComponent<Collider>();
			auto shape = check_onplayer_coll->add_shape<Sphere>();

			check_onplayer_go->transform->local_pos = Vector3(0, -3.0f, 0);

			check_onplayer_coll->physics_data.is_moveable = false;
			check_onplayer_coll->physics_data.is_hitable = false;
			check_onplayer_coll->tag = Collider_tags::Human;

			shape->r = 1;

			Waist->add_child(check_onplayer_go);
		}
		// respown時に接地判定を行う playerを包むsphere
		{
			auto check_respown_go = Gameobject_manager::create("check_respown_go", Scenelist::scene_player);
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

		player_mterial_0 = Material_manager::find_material(std::string("player_material_") + std::to_string(player_num) + std::string("_0"));
		player_mterial_1 = Material_manager::find_material(std::string("player_material_") + std::to_string(player_num) + std::string("_1"));

	}

	// 毎フレーム呼ばれる更新処理
	void Player::update()
	{
		if (stage_manager == nullptr)return; //対応するstageが無い時 return

		// playerのdragを初期値にする
		set_default_data();

		// respown処理
		if (check_respown() == true)return;

		// ongroundの情報の更新
		update_onground();

		// 体を起こす力を求める
		update_gnyat_pow();

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

	// このスクリプトがアタッチされているGOのactiveSelfがtrueになった時呼ばれる
	void Player::onEnable()
	{

	}

	// このスクリプトがアタッチされているGOのactiveSelfがfalseになった時呼ばれる
	void Player::onDisable()
	{

	}

}
