

#include "player_manager.h"
#include "../Adollib/Scripts/Main/Adollib.h"

#include "../Adollib/Scripts/Object/gameobject_manager.h"
#include "../Adollib/Scripts/Object/component_camera.h"

#include "../Adollib/Scripts/Imgui/imgui_all.h"
#include "../Adollib/Scripts/Imgui/debug.h"

#include "../Adollib/Scripts/Physics/ALP__physics_manager.h"
#include "../Adollib/Scripts/Physics/collider.h"
#include "../Adollib/Scripts/Physics/joint.h"

#include "../Adollib/Scripts/Renderer/material_manager.h"
#include "../Adollib/Scripts/Renderer/renderer_base.h"

#include "camera.h"
#include "player.h"
#include "input_changer.h"
#include "tutrial_manager.h"
#include "se_manager.h"

namespace Adollib
{
	// 所属するシーンの初期化時に一度だけ呼ばれる
	void Player_manager::awake() {

		//Vector3 player_start_pos = Vector3(-2.7f, 23, -150);
		//Vector3 player_start_pos = Vector3(-2.7f, 80, -5);
		//Vector3 player_start_pos = Vector3(-2.7f, 50, -5);

		//Quaternion player_start_rotate = quaternion_from_euler(20, 180, 0);

		//add_player(0, player_start_pos, Vector3(20, 180, 0), Vector3(1), Vector3(0.5f));
		//add_player(1, player_start_pos, Vector3(30, 180, 0), Vector3(1), Vector3(0.5f));
		//add_player(2, player_start_pos, Vector3(40, 180, 0), Vector3(1), Vector3(0.5f));


		//Vector3 player_start_pos = Vector3(-2.7f, 50, -5);

		//Quaternion player_start_rotate = quaternion_from_euler(20, 180, 0);

		//add_player(0, player_start_pos, Vector3(20, 180, 0), Vector3(1), Vector3(0.5f));

		//add_players(1);
	}


	// 毎フレーム呼ばれる更新処理
	void Player_manager::update()
	{

	}

	void Player_manager::set_stage_manager_ptr(Stage_manager* m) {
		for (int i = 0; i < 4; ++i) {
			if (players[i] == nullptr)continue;
			players[i]->set_stage_manager_ptr(m);
		}
	};
	void Player_manager::set_Tpause_and_set_transform(const Vector3& position, const Quaternion& orient) {
		for (int i = 0; i < 4; ++i) {
			if (players[i] == nullptr)continue;
			players[i]->set_Tpause(position, orient);
		}
	}
	void Player_manager::set_moveable(bool is_moveable) {
		for (int i = 0; i < 4; ++i) {
			if (players[i] == nullptr)continue;
			players[i]->set_moveable(is_moveable);
		}
	}
	void Player_manager::set_shadow_camera_pos(const Vector3& pos) {
		for (int i = 0; i < 4; ++i) {
			if (players[i] == nullptr)continue;
			players[i]->set_shadow_camera_pos(pos);
		}
	}
	void Player_manager::set_shadow_camera_dir(const Vector3& dir) {
		for (int i = 0; i < 4; ++i) {
			if (players[i] == nullptr)continue;
			players[i]->set_shadow_camera_dir(dir);
		}
	}
	void Player_manager::set_is_shotable(bool is) {
		for (int i = 0; i < 4; ++i) {
			if (players[i] == nullptr)continue;
			players[i]->set_is_shotable(is);
		}
	}
	void Player_manager::player_respown() {
		for (int i = 0; i < 4; ++i) {
			if (players[i] == nullptr)continue;
			players[i]->respown();
		}
	}


	void Player_manager::add_players(int num) {
		if (num == 0 || num > 4)return;

		Vector3 player_start_pos = Vector3(-2.7f, 50, -5);

		Quaternion player_start_rotate = quaternion_from_euler(20, 180, 0);

		const float S_W = Al_Global::SCREEN_WIDTH;
		const float S_H = Al_Global::SCREEN_HEIGHT;

		const float S_W_inv2 = Al_Global::SCREEN_WIDTH * 0.5f;
		const float S_H_inv2 = Al_Global::SCREEN_HEIGHT * 0.5f;

		const float S_W_inv3 = Al_Global::SCREEN_WIDTH / 3.f;
		const float S_H_inv3 = Al_Global::SCREEN_HEIGHT / 3.f;

		const float S_W_inv4 = Al_Global::SCREEN_WIDTH * 0.25f;
		const float S_H_inv4 = Al_Global::SCREEN_HEIGHT * 0.25f;


		const UI_data base_ui_datas[4][4]{
			{
				UI_data(0, 0, S_W, S_H, 0, 0, S_W, S_H),
				UI_data(),
				UI_data(),
				UI_data(),
			},

			{
				UI_data(S_W_inv2 * 0, 0, S_W_inv2, S_H, S_W_inv4, 0, S_W_inv2, S_H),
				UI_data(S_W_inv2 * 1, 0, S_W_inv2, S_H, S_W_inv4, 0, S_W_inv2, S_H),
				UI_data(),
				UI_data(),
			},

			{
				UI_data(S_W_inv3 * 0, 0, S_W_inv3, S_H,  S_W_inv3, 0, S_W_inv3, S_H),
				UI_data(S_W_inv3 * 1, 0, S_W_inv3, S_H,  S_W_inv3, 0, S_W_inv3, S_H),
				UI_data(S_W_inv3 * 2, 0, S_W_inv3, S_H,  S_W_inv3, 0, S_W_inv3, S_H),
				UI_data(),
			},

			{
				UI_data(S_W_inv2 * 0, S_H_inv2 * 0, S_W_inv2, S_H_inv2,  0, 0, S_W, S_H),
				UI_data(S_W_inv2 * 1, S_H_inv2 * 0, S_W_inv2, S_H_inv2,  0, 0, S_W, S_H),
				UI_data(S_W_inv2 * 0, S_H_inv2 * 1, S_W_inv2, S_H_inv2,  0, 0, S_W, S_H),
				UI_data(S_W_inv2 * 1, S_H_inv2 * 1, S_W_inv2, S_H_inv2,  0, 0, S_W, S_H),
			}


		};


		float tutrial_ui_x[4]{
			-63,
			-13.2f,
			-63,
			-63
		};


		Player* players[2];
		for (int i = 0; i < num; ++i) {

			players[i] = add_player(i, player_start_pos + Vector3(10, 0, 0) * i, Vector3(20, 180, 0), Vector3(1), Vector3(0.5f),
				base_ui_datas[num - 1][i], tutrial_ui_x[num - 1]
			);
		}

		if (num == 2) {
			players[0]->another_player_ptr = players[1];
			players[1]->another_player_ptr = players[0];
		}
	}

	void Player_manager::delete_players() {
		for (int i = 0; i < 4; ++i) {
			if (players[i] == nullptr)continue;
			Gameobject_manager::deleteGameobject(players[i]->gameobject);
		}

		for (int i = 0; i < 4; ++i) {
			players[i] = nullptr;
		}
	}

	Player* Player_manager::add_player(int player_num, const Vector3& position, const Vector3& rotate, const Vector3& main_color, const Vector3& sub_color, const UI_data& camera_data, float tutrial_ui_x) {

		Player*& player = players[player_num];

		Gameobject* Human = Gameobject_manager::create("Human", Scenelist::scene_player);
		Human->transform->local_pos = position;
		Human->transform->local_orient = quaternion_from_euler(rotate);

		auto player_material = Material_manager::create_material(std::string("player_material_") + std::to_string(player_num) + std::string("_0"));
		player_material->Load_VS("./DefaultShader/dither_noshadow_vs.cso");
		player_material->Load_PS("./DefaultShader/dither_noshadow_ps.cso");
		player_material->create_constantbuffer(6, 1);
		player_material->color = Vector4(main_color, 1);

		//::: Gameobjectの生成 :::
		Gameobject* Head = Gameobject_manager::createCube("Head", Scenelist::scene_player);

		Gameobject* Lsholder = Gameobject_manager::createCube("Lsholder", Scenelist::scene_player);
		Gameobject* Lelbow = Gameobject_manager::createCube("Lelbow", Scenelist::scene_player);
		Gameobject* Lhand = Gameobject_manager::create("Lhand", Scenelist::scene_player);
		Gameobject* Rsholder = Gameobject_manager::createCube("Rsholder", Scenelist::scene_player);
		Gameobject* Relbow = Gameobject_manager::createCube("Relbow", Scenelist::scene_player);
		Gameobject* Rhand = Gameobject_manager::create("Rhand", Scenelist::scene_player);
		Gameobject* Body = Gameobject_manager::createCube("Body", Scenelist::scene_player);
		Gameobject* Waist = Gameobject_manager::createCube("Waist", Scenelist::scene_player);
		Gameobject* Rleg = Gameobject_manager::createCube("Rleg", Scenelist::scene_player);
		Gameobject* Rfoot = Gameobject_manager::createCube("Rfoot", Scenelist::scene_player);
		Gameobject* Lleg = Gameobject_manager::createCube("Lleg", Scenelist::scene_player);
		Gameobject* Lfoot = Gameobject_manager::createCube("Lfoot", Scenelist::scene_player);

		Head->findComponent<Renderer_base>()->set_material(player_material);
		Lsholder->findComponent<Renderer_base>()->set_material(player_material);
		Lelbow->findComponent<Renderer_base>()->set_material(player_material);
		Rsholder->findComponent<Renderer_base>()->set_material(player_material);
		Relbow->findComponent<Renderer_base>()->set_material(player_material);
		Body->findComponent<Renderer_base>()->set_material(player_material);
		Waist->findComponent<Renderer_base>()->set_material(player_material);
		Rleg->findComponent<Renderer_base>()->set_material(player_material);
		Rfoot->findComponent<Renderer_base>()->set_material(player_material);
		Lleg->findComponent<Renderer_base>()->set_material(player_material);
		Lfoot->findComponent<Renderer_base>()->set_material(player_material);

		//::: collider,shapeのアタッチ :::
		Collider* Head_collider = Head->addComponent<Collider>();
		Collider* Lsholder_collider = Lsholder->addComponent<Collider>();
		Collider* Lelbow_collider = Lelbow->addComponent<Collider>();
		Collider* Lhand_collider = Lhand->addComponent<Collider>();
		Collider* Rsholder_collider = Rsholder->addComponent<Collider>();
		Collider* Relbow_collider = Relbow->addComponent<Collider>();
		Collider* Rhand_collider = Rhand->addComponent<Collider>();
		Collider* Body_collider = Body->addComponent<Collider>();
		Collider* Waist_collider = Waist->addComponent<Collider>();
		Collider* Rleg_collider = Rleg->addComponent<Collider>();
		Collider* Rfoot_collider = Rfoot->addComponent<Collider>();
		Collider* Lleg_collider = Lleg->addComponent<Collider>();
		Collider* Lfoot_collider = Lfoot->addComponent<Collider>();


		Sphere* Head_shape = Head_collider->add_shape<Sphere>();
		Sphere* Lsholder_shape = Lsholder_collider->add_shape<Sphere>();
		Sphere* Lelbow_shape = Lelbow_collider->add_shape<Sphere>();
		Sphere* Lhand_shape = Lhand_collider->add_shape<Sphere>();
		Sphere* Rsholder_shape = Rsholder_collider->add_shape<Sphere>();
		Sphere* Relbow_shape = Relbow_collider->add_shape<Sphere>();
		Sphere* Rhand_shape = Rhand_collider->add_shape<Sphere>();
		Box* Body_shape = Body_collider->add_shape<Box>();
		Box* Waist_shape = Waist_collider->add_shape<Box>();
		//Sphere* Waist_shape  = Waist_collider->add_shape<Sphere>();
		Sphere* Rleg_shape = Rleg_collider->add_shape<Sphere>();
		Sphere* Rfoot_shape = Rfoot_collider->add_shape<Sphere>();
		Sphere* Lleg_shape = Lleg_collider->add_shape<Sphere>();
		Sphere* Lfoot_shape = Lfoot_collider->add_shape<Sphere>();

		BallJoint* Lhand_joint = nullptr;
		BallJoint* Rhand_joint = nullptr;

		{
			//::: 親子関係の設定 :::
			Human->add_child(Body);

			Human->add_child(Head);

			Human->add_child(Lsholder);
			Human->add_child(Lelbow);
			Human->add_child(Lhand);
			Human->add_child(Rsholder);
			Human->add_child(Relbow);
			Human->add_child(Rhand);

			Human->add_child(Waist);

			Human->add_child(Rleg);
			Human->add_child(Rfoot);
			Human->add_child(Lleg);
			Human->add_child(Lfoot);

			//::: gameobjectの座標設定 :::
			Vector3 head_size = Vector3(1, 1, 1);
			Vector3 body_size = Vector3(0.8f, 0.6f, 0.8f);
			Vector3 Waist_size = Vector3(0.81f, 0.7f, 0.81f);
			{
				{
					Head->transform->local_scale = head_size;
					Head->transform->local_pos = Vector3(0, body_size.y + head_size.y + 0.2f, 0);
				}
			}

			{
				{
					auto& GO = Body;
					GO->transform->local_scale = body_size;
					GO->transform->local_orient = quaternion_from_euler(0, 0, 0);
					GO->transform->local_pos = Vector3(0, 0, 0);
				}
				{
					auto& GO = Waist;
					GO->transform->local_scale = Waist_size;
					GO->transform->local_pos = Vector3(0, -0.7f * 2, 0);
				}
			}

			Vector3 sholder_size = Vector3(0.35f, 0.6f, 0.4f);
			Vector3 arm_size = sholder_size;
			Vector3 hand_size = Vector3(0.7f, 0.7f, 0.7f);
			float arm_y_pos = 0.18f;
			{
				{
					auto& GO = Lsholder;
					GO->transform->local_scale = sholder_size;
					GO->transform->local_orient = quaternion_from_euler(0, +30, +90);
					//GO->transform->local_pos = Vector3(+(body_size.x + arm_size.y * 1 + 0.1f), arm_y_pos, 0);
					GO->transform->local_pos = Vector3(+1.62f, arm_y_pos, -0.30f);
				}
				{
					auto& GO = Lelbow;
					GO->transform->local_scale = arm_size;
					GO->transform->local_orient = quaternion_from_euler(0, +30, +90);
					GO->transform->local_pos = Vector3(+(body_size.x + arm_size.y * 3 + 0.2f), arm_y_pos, -0.98f);
				}
				{
					auto& GO = Lhand;
					GO->transform->local_scale = hand_size;
					GO->transform->local_orient = quaternion_from_euler(0, 0, 0);
					GO->transform->local_pos = Vector3(+(body_size.x + arm_size.y * 4 - 0.2f), arm_y_pos - 0.15f, -1.07f);
				}
				{
					auto& GO = Rsholder;
					GO->transform->local_scale = sholder_size;
					GO->transform->local_orient = quaternion_from_euler(0, -30, -90);
					//GO->transform->local_pos = Vector3(1.08f, arm_y_pos, 1.86f);
					//GO->transform->local_pos = Vector3(3.78f, arm_y_pos, 4.02f);
					GO->transform->local_pos = Vector3(-1.62f, arm_y_pos, -0.30f);
				}
				{
					auto& GO = Relbow;
					GO->transform->local_scale = arm_size;
					GO->transform->local_orient = quaternion_from_euler(0, -30, -90);
					GO->transform->local_pos = Vector3(-(body_size.x + arm_size.y * 3 + 0.2f), arm_y_pos, -0.98f);
				}
				{
					auto& GO = Rhand;
					GO->transform->local_scale = hand_size;
					GO->transform->local_orient = quaternion_from_euler(0, 0, 0);
					GO->transform->local_pos = Vector3(-(body_size.x + arm_size.y * 4 - 0.2f), arm_y_pos - 0.15f, -1.07f);
				}
			}

			Vector3 Leg_size = Vector3(0.3f, 0.15f, 0.3f);
			Vector3 Foot_size = Vector3(0.4f, 0.25f, 0.4f);
			float leg_x_pos = 0.6f;
			{
				{
					auto& GO = Rleg;
					GO->transform->local_scale = Foot_size;
					GO->transform->local_orient = quaternion_from_euler(0, 0, 0);
					GO->transform->local_pos = Vector3(-leg_x_pos, -(body_size.y + Waist_size.y * 2 + Foot_size.y * 2), 0);
				}
				{
					auto& GO = Rfoot;
					GO->transform->local_scale = Foot_size;
					GO->transform->local_orient = quaternion_from_euler(0, 0, 0);
					GO->transform->local_pos = Vector3(-leg_x_pos, -(body_size.y + Waist_size.y * 2 + Foot_size.y * 5), 0);
				}
				{
					auto& GO = Lleg;
					GO->transform->local_scale = Foot_size;
					GO->transform->local_orient = quaternion_from_euler(0, 0, 0);
					GO->transform->local_pos = Vector3(+leg_x_pos, -(body_size.y + Waist_size.y * 2 + Foot_size.y * 2), 0);
				}
				{
					auto& GO = Lfoot;
					GO->transform->local_scale = Foot_size;
					GO->transform->local_orient = quaternion_from_euler(0, 0, 0);
					GO->transform->local_pos = Vector3(+leg_x_pos, -(body_size.y + Waist_size.y * 2 + Foot_size.y * 5), 0);
				}
			}

			//::: 慣性モーメントの調整 :::
			Head_collider->set_barycenter(Vector3(0, +0.5f, 0));
			//Body_collider	 ->set_barycenter(Vector3(0, -0.2f, 0));
			Body_collider->set_barycenter(Vector3(0, +0.5f, 0));

			Lhand_collider->physics_data.inertial_mass = 100;
			Lhand_collider->set_tensor(Lhand_collider->get_tensor());
			Rhand_collider->physics_data.inertial_mass = 100;
			Rhand_collider->set_tensor(Lhand_collider->get_tensor());

			Lsholder_collider->set_barycenter(Vector3(0, 0.5f, 0));
			Rsholder_collider->set_barycenter(Vector3(0, 0.5f, 0));
			Lelbow_collider->set_barycenter(Vector3(0, 0.5f, 0));
			Relbow_collider->set_barycenter(Vector3(0, 0.5f, 0));

			Lleg_collider->set_barycenter(Vector3(0, 1, 0));
			Rleg_collider->set_barycenter(Vector3(0, 1, 0));
			Lfoot_collider->set_barycenter(Vector3(0, 1, 0));
			Rfoot_collider->set_barycenter(Vector3(0, 1, 0));

			//::: 質量の調整 :::
			Head_collider->physics_data.inertial_mass = 3;
			Lsholder_collider->physics_data.inertial_mass = 4;
			Rsholder_collider->physics_data.inertial_mass = 4;
			Lelbow_collider->physics_data.inertial_mass = 3;
			Relbow_collider->physics_data.inertial_mass = 3;
			Lhand_collider->physics_data.inertial_mass = 1.5f;
			Rhand_collider->physics_data.inertial_mass = 1.5f;
			Body_collider->physics_data.inertial_mass = 5;
			Waist_collider->physics_data.inertial_mass = 4;
			Lleg_collider->physics_data.inertial_mass = 2;
			Rleg_collider->physics_data.inertial_mass = 2;
			Lfoot_collider->physics_data.inertial_mass = 1;
			Rfoot_collider->physics_data.inertial_mass = 1;

			Head_collider->physics_data.anglar_drag = 0.5f;
			Lsholder_collider->physics_data.anglar_drag = 0.5f;
			Lelbow_collider->physics_data.anglar_drag = 0.5f;
			Lhand_collider->physics_data.anglar_drag = 1;
			Rsholder_collider->physics_data.anglar_drag = 0.5f;
			Relbow_collider->physics_data.anglar_drag = 0.5f;
			Rhand_collider->physics_data.anglar_drag = 1;
			Waist_collider->physics_data.anglar_drag = 0.8f;

			Head_collider->physics_data.restitution = 0.01f;
			Lsholder_collider->physics_data.restitution = 0.01f;
			Rsholder_collider->physics_data.restitution = 0.01f;
			Lelbow_collider->physics_data.restitution = 0.01f;
			Relbow_collider->physics_data.restitution = 0.01f;
			Lhand_collider->physics_data.restitution = 0.01f;
			Rhand_collider->physics_data.restitution = 0.01f;
			Body_collider->physics_data.restitution = 0.01f;
			Waist_collider->physics_data.restitution = 0.01f;
			Lleg_collider->physics_data.restitution = 0.01f;
			Rleg_collider->physics_data.restitution = 0.01f;
			Lfoot_collider->physics_data.restitution = 0.01f;
			Rfoot_collider->physics_data.restitution = 0.01f;

			Head_collider->physics_data.dynamic_friction = 0.01f;
			Lsholder_collider->physics_data.dynamic_friction = 0.01f;
			Rsholder_collider->physics_data.dynamic_friction = 0.01f;
			Lelbow_collider->physics_data.dynamic_friction = 0.01f;
			Relbow_collider->physics_data.dynamic_friction = 0.01f;
			Lhand_collider->physics_data.dynamic_friction = 0.01f;
			Rhand_collider->physics_data.dynamic_friction = 0.01f;
			Body_collider->physics_data.dynamic_friction = 0.01f;
			Waist_collider->physics_data.dynamic_friction = 0.01f;
			Lleg_collider->physics_data.dynamic_friction = 0.01f;
			Rleg_collider->physics_data.dynamic_friction = 0.01f;
			Lfoot_collider->physics_data.dynamic_friction = 0.01f;
			Rfoot_collider->physics_data.dynamic_friction = 0.01f;

			//::: 重力適用の調整 :::
			Head_collider->physics_data.is_fallable = false;
			//Rsholder_collider->physics_data.is_fallable = false;
			//Lsholder_collider->physics_data.is_fallable = false;
			Rleg_collider->physics_data.is_fallable = false;
			Lleg_collider->physics_data.is_fallable = false;

			//::: tagの設定 :::
			Collider_tagbit tag = 0;
			if (player_num == 0) tag = Collider_tags::Player00;
			if (player_num == 1) tag = Collider_tags::Player01;
			if (player_num == 2) tag = Collider_tags::Player02;
			if (player_num == 3) tag = Collider_tags::Player03;

			Head_collider->tag = Collider_tags::Human | tag;
			Lsholder_collider->tag = Collider_tags::Human | tag;
			Lelbow_collider->tag = Collider_tags::Human | tag;
			Lhand_collider->tag = Collider_tags::Human | tag;
			Rsholder_collider->tag = Collider_tags::Human | tag;
			Relbow_collider->tag = Collider_tags::Human | tag;
			Rhand_collider->tag = Collider_tags::Human | tag;
			Body_collider->tag = Collider_tags::Human | tag;
			Waist_collider->tag = Collider_tags::Human | tag;
			Rleg_collider->tag = Collider_tags::Human | tag;
			Rfoot_collider->tag = Collider_tags::Human | tag;
			Lleg_collider->tag = Collider_tags::Human | tag;
			Lfoot_collider->tag = Collider_tags::Human | tag;

			Lhand_collider->ignore_tags |= tag;
			Rhand_collider->ignore_tags |= tag;


			//::: sphereの調整 :::
			Lleg_shape->r = Lleg->transform->local_scale.y * 2;
			Lfoot_shape->r = Lfoot->transform->local_scale.y * 2;
			Rleg_shape->r = Rleg->transform->local_scale.y * 2;
			Rfoot_shape->r = Rfoot->transform->local_scale.y * 2;

			// 立ち上がりやすいように waistの当たり判定を調整
			Waist_collider->set_tensor(Waist_collider->get_tensor());
			Waist_shape->center.y += Waist->transform->local_scale.y * 0.5f;
			Waist_shape->size.y = 0.64f;
			Waist_shape->size.x *= 0.9f;
			Waist_shape->size.z *= 0.9f;

			//Waist_shape->center.y += Waist->transform->local_scale.y * 0.1f;
			//Waist_shape->size.y = 0.3f;
			//Waist_shape->size.x *= 0.9f;
			//Waist_shape->size.z *= 0.9f;

			Lhand_collider->physics_data.dynamic_friction = 0;
			Rhand_collider->physics_data.dynamic_friction = 0;


			//Head_collider    ->physics_data.is_hitable = false;
			//Lsholder_collider->physics_data.is_hitable = false;
			//Rsholder_collider->physics_data.is_hitable = false;
			//Lelbow_collider  ->physics_data.is_hitable = false;
			//Relbow_collider  ->physics_data.is_hitable = false;
			//Lhand_collider   ->physics_data.is_hitable = false;
			//Rhand_collider   ->physics_data.is_hitable = false;
			//Body_collider    ->physics_data.is_hitable = false;
			//Waist_collider   ->physics_data.is_hitable = false;
			//Lleg_collider    ->physics_data.is_hitable = false;
			//Rleg_collider    ->physics_data.is_hitable = false;
			//Lfoot_collider   ->physics_data.is_hitable = false;
			//Rfoot_collider   ->physics_data.is_hitable = false;

			//Head_collider    ->physics_data.is_moveable = false;
			//Lsholder_collider->physics_data.is_moveable = false;
			//Rsholder_collider->physics_data.is_moveable = false;
			//Lelbow_collider  ->physics_data.is_moveable = false;
			//Relbow_collider  ->physics_data.is_moveable = false;
			//Lhand_collider   ->physics_data.is_moveable = false;
			//Rhand_collider   ->physics_data.is_moveable = false;
			//Body_collider    ->physics_data.is_moveable = false;
			//Waist_collider   ->physics_data.is_moveable = false;
			//Lleg_collider    ->physics_data.is_moveable = false;
			//Rleg_collider    ->physics_data.is_moveable = false;
			//Lfoot_collider   ->physics_data.is_moveable = false;
			//Rfoot_collider   ->physics_data.is_moveable = false;

			//::: Jointの設定
			{
				float joint_bias = 0.1f;
				float limit_bias = 0.1f;
				//頭
				{
					auto Cone = Joint::add_Conejoint(Head_collider, Body_collider, Vector3(0, -1, 0), Vector3(0, 0.8f, 0), Vector3(0, 1, 0), Vector3(0, 1, 0), joint_bias);
					Cone->limit = 30;
					Cone->limit_bias = limit_bias;

					auto Twist = Joint::add_Twistjoint(Body_collider, Head_collider, Vector3(0, 1, 0), Vector3(0, 1, 0), joint_bias);
					Twist->limit = Vector2(360 - 90, 90);
					Twist->limit_bias = limit_bias;
				}
				//腕
				{
					auto Cone = Joint::add_Conejoint(Body_collider, Lsholder_collider, Vector3(+1.1f, 0.2f, 0), Vector3(0, arm_size.y, 0), Vector3(-1, 0, -1).unit_vect(), Vector3(0, -1, 0).unit_vect(), joint_bias);
					Cone->limit = 85;
					Cone->limit_bias = limit_bias;
					//auto Cone = Joint::add_balljoint(Body_collider, Lsholder_collider, Vector3(+1.1f, 0.2f, 0), Vector3(0, arm_size.y, 0), joint_bias);

					auto Twist = Joint::add_Twistjoint(Body_collider, Lsholder_collider, Vector3(-1, 0, 0), Vector3(0, 1, 0), joint_bias);
					Twist->limit = Vector2(360 - 90, 90);
					Twist->limit_bias = limit_bias;
				}
				{
					auto hinge = Joint::add_Hingejoint(Lsholder_collider, Lelbow_collider, Vector3(-1, -(arm_size.y + 0.1f), 0), Vector3(+1, -(arm_size.y + 0.1f), 0), Vector3(-1, (arm_size.y + 0.1f), 0), Vector3(+1, (arm_size.y + 0.1f), 0), joint_bias);
					hinge->shrink_bias = 0.5f;
					hinge->stretch_bias = 0.5f;
					hinge->limit = Vector2(230, 360);
					hinge->limit_bias = limit_bias;
				}
				{
					auto ball = Joint::add_balljoint(Lelbow_collider, Lhand_collider, Vector3(-0.2f, -0.15f, 0), Vector3(0, 0, 0), joint_bias);
					Lhand_joint = ball;
				}
				{
					auto Cone = Joint::add_Conejoint(Body_collider, Rsholder_collider, Vector3(-1.1f, 0.2f, 0), Vector3(0, arm_size.y, 0), Vector3(+1, 0, -1).unit_vect(), Vector3(0, -1, 0).unit_vect(), joint_bias);
					Cone->limit = 85;
					Cone->limit_bias = limit_bias;

					auto Twist = Joint::add_Twistjoint(Body_collider, Rsholder_collider, Vector3(+1, 0, 0), Vector3(0, 1, 0), joint_bias);
					Twist->limit = Vector2(360 - 90, 90);
					Twist->limit_bias = limit_bias;
				}
				{
					auto hinge = Joint::add_Hingejoint(Rsholder_collider, Relbow_collider, Vector3(-1, -(arm_size.y + 0.1f), 0), Vector3(+1, -(arm_size.y + 0.1f), 0), Vector3(-1, (arm_size.y + 0.1f), 0), Vector3(+1, (arm_size.y + 0.1f), 0), joint_bias);
					hinge->shrink_bias = 0.5f;
					hinge->stretch_bias = 0.5f;
					hinge->limit = Vector2(230, 360);
					hinge->limit_bias = limit_bias;
				}
				{
					auto ball = Joint::add_balljoint(Relbow_collider, Rhand_collider, Vector3(0.2f, -0.15f, 0), Vector3(0, 0, 0), joint_bias);
					Rhand_joint = ball;
				}
				//腰
				{
					auto Cone = Joint::add_Conejoint(Body_collider, Waist_collider, Vector3(0, -1.0f, 0), Vector3(0, 0.5f, 0), Vector3(0, 1, 0).unit_vect(), Vector3(0, 1.5f, -1).unit_vect(), 0.1f);
					Cone->limit = 60;
					Cone->limit_bias = limit_bias;

					auto Twist = Joint::add_Twistjoint(Body_collider, Waist_collider, Vector3(0, 1, 0), Vector3(0, 1, 0), joint_bias);
					Twist->limit = Vector2(360 - 30, 30);
					Twist->limit_bias = limit_bias;
				}
				//足
				{
					auto Cone = Joint::add_Conejoint(Waist_collider, Rleg_collider, Vector3(-0.6f, -0.8f, 0), Vector3(0, 0.3f, 0), Vector3(0, -1, -1.02f).unit_vect(), Vector3(0, -1, 0).unit_vect(), joint_bias);
					Cone->limit = 80;
					Cone->limit_bias = limit_bias;

					auto Twist = Joint::add_Twistjoint(Waist_collider, Rleg_collider, Vector3(0, 1, 0), Vector3(0, 1, 0), joint_bias);
					Twist->limit = Vector2(360 - 5, 15);
					Twist->limit_bias = limit_bias;
				}
				{
					auto hinge = Joint::add_Hingejoint(Rleg_collider, Rfoot_collider, Vector3(-1, -0.4f, 0), Vector3(+1, -0.3f, 0), Vector3(-1, +0.3f, 0), Vector3(+1, +0.3f, 0), joint_bias);
					hinge->limit = Vector2(0, 60);
					hinge->limit_bias = limit_bias;
				}
				{
					auto Cone = Joint::add_Conejoint(Waist_collider, Lleg_collider, Vector3(+0.6f, -0.8f, 0), Vector3(0, 0.3f, 0), Vector3(0, -1, -1.02f).unit_vect(), Vector3(0, -1, 0).unit_vect(), joint_bias);
					Cone->limit = 80;
					Cone->limit_bias = limit_bias;

					auto Twist = Joint::add_Twistjoint(Waist_collider, Lleg_collider, Vector3(0, 1, 0), Vector3(0, 1, 0), joint_bias);
					Twist->limit = Vector2(360 - 15, 5);
					Twist->limit_bias = limit_bias;
				}
				{
					auto hinge = Joint::add_Hingejoint(Lleg_collider, Lfoot_collider, Vector3(-1, -0.4f, 0), Vector3(+1, -0.3f, 0), Vector3(-1, +0.3f, 0), Vector3(+1, +0.3f, 0), joint_bias);
					hinge->limit = Vector2(0, 60);
					hinge->limit_bias = limit_bias;
				}
			}

		}

		//顔とかベルトを着けてみる
		{
			auto player_material_02 = Material_manager::create_material(std::string("player_material_") + std::to_string(player_num) + std::string("_1"));
			player_material_02->color = Vector4(sub_color, 1);
			player_material_02->Load_VS("./DefaultShader/dither_noshadow_vs.cso");
			player_material_02->Load_PS("./DefaultShader/dither_noshadow_ps.cso");
			player_material_02->create_constantbuffer(6, 1);
			{
				Gameobject* eye0 = Gameobject_manager::createSphere("eye0", Scenelist::scene_player);
				Head->add_child(eye0);
				eye0->transform->local_pos = Vector3(+0.5f, 0.5f, -1);
				eye0->transform->local_scale = Vector3(0.25f, 0.25f, 0.25f);
				eye0->findComponent<Renderer_base>()->set_material(player_material_02);
			}
			{
				Gameobject* eye1 = Gameobject_manager::createSphere("eye1", Scenelist::scene_player);
				Head->add_child(eye1);
				eye1->transform->local_pos = Vector3(-0.5f, 0.5f, -1);
				eye1->transform->local_scale = Vector3(0.25f, 0.25f, 0.25f);
				eye1->findComponent<Renderer_base>()->set_material(player_material_02);
			}
			{
				Gameobject* mouth = Gameobject_manager::createCube("mouth", Scenelist::scene_player);
				Head->add_child(mouth);
				mouth->transform->local_pos = Vector3(0, -0.45f, -1);
				mouth->transform->local_scale = Vector3(0.7f, 0.25f, 0.3f);
				mouth->findComponent<Renderer_base>()->set_material(player_material_02);
			}

			{
				Gameobject* belt = Gameobject_manager::createCube("belt", Scenelist::scene_player);
				Waist->add_child(belt);
				belt->transform->local_pos = Vector3(0, -0.45f, 0);
				belt->transform->local_scale = Vector3(1.1f, 0.25f, 1.1f);
				belt->findComponent<Renderer_base>()->set_material(player_material_02);
			}
		}

		{
			Collider_tagbit tag = 0;
			if (player_num == 0) tag = Collider_tags::Player00;
			if (player_num == 1) tag = Collider_tags::Player01;
			if (player_num == 2) tag = Collider_tags::Player02;
			if (player_num == 3) tag = Collider_tags::Player03;

			auto GO = Gameobject_manager::create("player", Scenelist::scene_player);
			player = GO->addComponent<Player>();
			GO->add_child(Human);

			player->set_player_colliders(
				Head_collider,
				Lsholder_collider,
				Lelbow_collider,
				Lhand_collider,
				Rsholder_collider,
				Relbow_collider,
				Rhand_collider,
				Body_collider,
				Waist_collider,
				Rleg_collider,
				Rfoot_collider,
				Lleg_collider,
				Lfoot_collider
			);

			player->set_player_joints(
				Lhand_joint,
				Rhand_joint
			);

			player->player_collider_tag = tag;

			// 入力変換
			auto input_changer = GO->addComponent<Input_changer>();
			input_changer->pad_num = player_num;

			// tutrial
			auto tutrial = GO->addComponent<Tutrial_manager>();
			tutrial->tutrial_ui_x = tutrial_ui_x;

			// camera
			Gameobject* camera_go = Gameobject_manager::create("camera", Scenelist::scene_player);
			auto camera_comp = camera_go->addComponent<Camera>();
			auto camera_ = camera_go->addComponent<Camera_component>();
			camera_->ui_data = camera_data;
			GO->add_child(camera_go);

			// se
			auto se = GO->addComponent<SE_manager>();
			se->player_num = player_num;

			player->set_input_changerptr(input_changer);
			player->set_cameraptr(camera_comp);
			player->set_se_managerptr(se);
			player->set_player_num(player_num);

			camera_comp->set_input_changerptr(input_changer);
			camera_comp->set_player_transformptr(Waist->transform);

			//camera->is_active = false;

			return player;
		}


	}

}