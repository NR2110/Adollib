

#include "player_manager.h"
#include "../Adollib/Scripts/Object/gameobject_manager.h"

#include "../Adollib/Scripts/Imgui/imgui_all.h"
#include "../Adollib/Scripts/Imgui/debug.h"

#include "../Adollib/Scripts/Physics/ALP__physics_manager.h"
#include "../Adollib/Scripts/Physics/collider.h"
#include "../Adollib/Scripts/Physics/joint.h"

#include "../Adollib/Scripts/Renderer/material_manager.h"
#include "../Adollib/Scripts/Renderer/renderer_base.h"

#include "player.h"
#include "input_changer.h"

namespace Adollib
{
	// 所属するシーンの初期化時に一度だけ呼ばれる
	void Player_manager::awake() {

		//Vector3 player_start_pos = Vector3(-2.7f, 23, -150);
		//Vector3 player_start_pos = Vector3(-2.7f, 80, -5);
		Vector3 player_start_pos = Vector3(-2.7f, 50, -5);

		Quaternion player_start_rotate = quaternion_from_euler(20, 180, 0);

		add_player(0, player_start_pos, Vector3(20, 180, 0), Vector3(1), Vector3(0.5f));

	}

	// 毎フレーム呼ばれる更新処理
	void Player_manager::update()
	{

	}


	void Player_manager::add_player(int player_num, const Vector3& position, const Vector3& rotate, const Vector3& main_color, const Vector3& sub_color) {

		Player*& player = players[player_num];

		Gameobject* Human = Gameobject_manager::create("Human");
		Human->transform->local_pos = position;
		Human->transform->local_orient = quaternion_from_euler(rotate);

		auto player_material_01 = Material_manager::create_material("player_material_01");
		player_material_01->Load_PS("./DefaultShader/default_ps_dither_noshadow.cso");
		player_material_01->color = Vector4(main_color, 1);

		//::: Gameobjectの生成 :::
		Gameobject* Head = Gameobject_manager::createCube("Head");

		Gameobject* Lsholder = Gameobject_manager::createCube("Lsholder");
		Gameobject* Lelbow   = Gameobject_manager::createCube("Lelbow");
		Gameobject* Lhand    = Gameobject_manager::create("Lhand");
		Gameobject* Rsholder = Gameobject_manager::createCube("Rsholder");
		Gameobject* Relbow   = Gameobject_manager::createCube("Relbow");
		Gameobject* Rhand    = Gameobject_manager::create("Rhand");
		Gameobject* Body     = Gameobject_manager::createCube("Body");
		Gameobject* Waist    = Gameobject_manager::createCube("Waist");
		Gameobject* Rleg     = Gameobject_manager::createCube("Rleg");
		Gameobject* Rfoot    = Gameobject_manager::createCube("Rfoot");
		Gameobject* Lleg     = Gameobject_manager::createCube("Lleg");
		Gameobject* Lfoot    = Gameobject_manager::createCube("Lfoot");

		Head	->findComponent<Renderer_base>()->set_material(player_material_01);
		Lsholder->findComponent<Renderer_base>()->set_material(player_material_01);
		Lelbow  ->findComponent<Renderer_base>()->set_material(player_material_01);
		Rsholder->findComponent<Renderer_base>()->set_material(player_material_01);
		Relbow  ->findComponent<Renderer_base>()->set_material(player_material_01);
		Body    ->findComponent<Renderer_base>()->set_material(player_material_01);
		Waist   ->findComponent<Renderer_base>()->set_material(player_material_01);
		Rleg    ->findComponent<Renderer_base>()->set_material(player_material_01);
		Rfoot   ->findComponent<Renderer_base>()->set_material(player_material_01);
		Lleg    ->findComponent<Renderer_base>()->set_material(player_material_01);
		Lfoot   ->findComponent<Renderer_base>()->set_material(player_material_01);

		//::: collider,shapeのアタッチ :::
		Collider* Head_collider     = Head->addComponent<Collider>();
		Collider* Lsholder_collider = Lsholder->addComponent<Collider>();
		Collider* Lelbow_collider   = Lelbow->addComponent<Collider>();
		Collider* Lhand_collider    = Lhand->addComponent<Collider>();
		Collider* Rsholder_collider = Rsholder->addComponent<Collider>();
		Collider* Relbow_collider   = Relbow->addComponent<Collider>();
		Collider* Rhand_collider    = Rhand->addComponent<Collider>();
		Collider* Body_collider     = Body->addComponent<Collider>();
		Collider* Waist_collider    = Waist->addComponent<Collider>();
		Collider* Rleg_collider     = Rleg->addComponent<Collider>();
		Collider* Rfoot_collider    = Rfoot->addComponent<Collider>();
		Collider* Lleg_collider     = Lleg->addComponent<Collider>();
		Collider* Lfoot_collider    = Lfoot->addComponent<Collider>();


		Sphere* Head_shape     = Head_collider->add_shape<Sphere>();
		Sphere* Lsholder_shape = Lsholder_collider->add_shape<Sphere>();
		Sphere* Lelbow_shape   = Lelbow_collider->add_shape<Sphere>();
		Sphere* Lhand_shape    = Lhand_collider->add_shape<Sphere>();
		Sphere* Rsholder_shape = Rsholder_collider->add_shape<Sphere>();
		Sphere* Relbow_shape   = Relbow_collider->add_shape<Sphere>();
		Sphere* Rhand_shape    = Rhand_collider->add_shape<Sphere>();
		Box* Body_shape        = Body_collider->add_shape<Box>();
		Box* Waist_shape       = Waist_collider->add_shape<Box>();
		//Sphere* Waist_shape  = Waist_collider->add_shape<Sphere>();
		Sphere* Rleg_shape     = Rleg_collider->add_shape<Sphere>();
		Sphere* Rfoot_shape    = Rfoot_collider->add_shape<Sphere>();
		Sphere* Lleg_shape     = Lleg_collider->add_shape<Sphere>();
		Sphere* Lfoot_shape    = Lfoot_collider->add_shape<Sphere>();

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
			Vector3 hand_size = Vector3(0.45f, 0.45f, 0.45f);
			float arm_y_pos = 0.18f;
			{
				{
					auto& GO = Lsholder;
					GO->transform->local_scale = sholder_size;
					GO->transform->local_orient = quaternion_from_euler(0, +30, +90);
					//GO->transform->local_pos = Vector3(+(body_size.x + arm_size.y * 1 + 0.1f), arm_y_pos, 0);
					GO->transform->local_pos = Vector3(+1.62, arm_y_pos, -0.30f);
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
					GO->transform->local_pos = Vector3(-1.62, arm_y_pos, -0.30f);
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

			Vector3 Leg_size = Vector3(0.3f, 0.15, 0.3f);
			Vector3 Foot_size = Vector3(0.4f, 0.25, 0.4f);
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

			//::: 重力適用の調整 :::
			Head_collider->physics_data.is_fallable = false;
			//Rsholder_collider->physics_data.is_fallable = false;
			//Lsholder_collider->physics_data.is_fallable = false;
			Rleg_collider->physics_data.is_fallable = false;
			Lleg_collider->physics_data.is_fallable = false;

			//::: tagの設定 :::
			Head_collider->tag = Collider_tags::Human;
			Lsholder_collider->tag = Collider_tags::Human;
			Lelbow_collider->tag = Collider_tags::Human;
			Lhand_collider->tag = Collider_tags::Human;
			Rsholder_collider->tag = Collider_tags::Human;
			Relbow_collider->tag = Collider_tags::Human;
			Rhand_collider->tag = Collider_tags::Human;
			Body_collider->tag = Collider_tags::Human;
			Waist_collider->tag = Collider_tags::Human;
			Rleg_collider->tag = Collider_tags::Human;
			Rfoot_collider->tag = Collider_tags::Human;
			Lleg_collider->tag = Collider_tags::Human;
			Lfoot_collider->tag = Collider_tags::Human;

			Lhand_collider->ignore_tags |= Collider_tags::Human;
			Rhand_collider->ignore_tags |= Collider_tags::Human;


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


			Head_collider    ->physics_data.is_hitable = false;
			Lsholder_collider->physics_data.is_hitable = false;
			Rsholder_collider->physics_data.is_hitable = false;
			Lelbow_collider  ->physics_data.is_hitable = false;
			Relbow_collider  ->physics_data.is_hitable = false;
			Lhand_collider   ->physics_data.is_hitable = false;
			Rhand_collider   ->physics_data.is_hitable = false;
			Body_collider    ->physics_data.is_hitable = false;
			Waist_collider   ->physics_data.is_hitable = false;
			Lleg_collider    ->physics_data.is_hitable = false;
			Rleg_collider    ->physics_data.is_hitable = false;
			Lfoot_collider   ->physics_data.is_hitable = false;
			Rfoot_collider   ->physics_data.is_hitable = false;

			Head_collider    ->physics_data.is_moveable = false;
			Lsholder_collider->physics_data.is_moveable = false;
			Rsholder_collider->physics_data.is_moveable = false;
			Lelbow_collider  ->physics_data.is_moveable = false;
			Relbow_collider  ->physics_data.is_moveable = false;
			Lhand_collider   ->physics_data.is_moveable = false;
			Rhand_collider   ->physics_data.is_moveable = false;
			Body_collider    ->physics_data.is_moveable = false;
			Waist_collider   ->physics_data.is_moveable = false;
			Lleg_collider    ->physics_data.is_moveable = false;
			Rleg_collider    ->physics_data.is_moveable = false;
			Lfoot_collider   ->physics_data.is_moveable = false;
			Rfoot_collider   ->physics_data.is_moveable = false;

			//::: Jointの設定
			float joint_bias = 0.1f;
			//頭
			{
				auto Ball = Joint::add_Conejoint(Head_collider, Body_collider, Vector3(0, -1, 0), Vector3(0, 0.8f, 0), Vector3(0, 1, 0), Vector3(0, 1, 0), joint_bias);
				Ball->limit = 30;

				auto Twist = Joint::add_Twistjoint(Body_collider, Head_collider, Vector3(0, 1, 0), Vector3(0, 1, 0), joint_bias);
				Twist->limit = Vector2(360 - 90, 90);
			}
			//腕
			{
				auto Cone = Joint::add_Conejoint(Body_collider, Lsholder_collider, Vector3(+1.1f, 0.2f, 0), Vector3(0, arm_size.y, 0), Vector3(-1, 0, -1).unit_vect(), Vector3(0, -1, 0).unit_vect(), joint_bias);
				Cone->limit = 85;

				auto Twist = Joint::add_Twistjoint(Body_collider, Lsholder_collider, Vector3(0, 1, 0), Vector3(0, 1, 0), joint_bias);
				Twist->limit = Vector2(360 - 90, 90);
			}
			{
				auto hinge = Joint::add_Hingejoint(Lsholder_collider, Lelbow_collider, Vector3(-1, -(arm_size.y + 0.1f), 0), Vector3(+1, -(arm_size.y + 0.1f), 0), Vector3(-1, (arm_size.y + 0.1f), 0), Vector3(+1, (arm_size.y + 0.1f), 0), joint_bias);
				hinge->shrink_bias = 0.5f;
				hinge->stretch_bias = 0.5f;
				hinge->limit = Vector2(230, 360);
			}
			{
				auto ball = Joint::add_balljoint(Lelbow_collider, Lhand_collider, Vector3(-0.2f, -0.15f, 0), Vector3(0, 0, 0), joint_bias);
				Lhand_joint = ball;
			}
			{
				auto Cone = Joint::add_Conejoint(Body_collider, Rsholder_collider, Vector3(-1.1f, 0.2f, 0), Vector3(0, arm_size.y, 0), Vector3(+1, 0, -1).unit_vect(), Vector3(0, -1, 0).unit_vect(), joint_bias);
				Cone->limit = 85;

				auto Twist = Joint::add_Twistjoint(Body_collider, Rsholder_collider, Vector3(0, 1, 0), Vector3(0, 1, 0), joint_bias);
				Twist->limit = Vector2(360 - 90, 90);
			}
			{
				auto hinge = Joint::add_Hingejoint(Rsholder_collider, Relbow_collider, Vector3(-1, -(arm_size.y + 0.1f), 0), Vector3(+1, -(arm_size.y + 0.1f), 0), Vector3(-1, (arm_size.y + 0.1f), 0), Vector3(+1, (arm_size.y + 0.1f), 0), joint_bias);
				hinge->shrink_bias = 0.5f;
				hinge->stretch_bias = 0.5f;
				hinge->limit = Vector2(230, 360);
			}
			{
				auto ball = Joint::add_balljoint(Relbow_collider, Rhand_collider, Vector3(0.2f, -0.15f, 0), Vector3(0, 0, 0), joint_bias);
				Rhand_joint = ball;
			}
			//腰
			{
				auto Cone = Joint::add_Conejoint(Body_collider, Waist_collider, Vector3(0, -1.0f, 0), Vector3(0, 0.5f, 0), Vector3(0, 1, 0).unit_vect(), Vector3(0, 1.5f, -1).unit_vect(), 0.1f);
				Cone->limit = 60;

				auto Twist = Joint::add_Twistjoint(Body_collider, Waist_collider, Vector3(0, 1, 0), Vector3(0, 1, 0), joint_bias);
				Twist->limit = Vector2(360 - 30, 30);
			}
			//足
			{
				auto Cone = Joint::add_Conejoint(Waist_collider, Rleg_collider, Vector3(-0.6f, -0.8f, 0), Vector3(0, 0.3, 0), Vector3(0, -1, -1.02f).unit_vect(), Vector3(0, -1, 0).unit_vect(), joint_bias);
				Cone->limit = 80;

				auto Twist = Joint::add_Twistjoint(Waist_collider, Rleg_collider, Vector3(0, 1, 0), Vector3(0, 1, 0), joint_bias);
				Twist->limit = Vector2(360 - 5, 15);
			}
			{
				auto hinge = Joint::add_Hingejoint(Rleg_collider, Rfoot_collider, Vector3(-1, -0.4f, 0), Vector3(+1, -0.3f, 0), Vector3(-1, +0.3f, 0), Vector3(+1, +0.3f, 0), joint_bias);
				hinge->limit = Vector2(0, 60);
			}
			{
				auto Cone = Joint::add_Conejoint(Waist_collider, Lleg_collider, Vector3(+0.6f, -0.8f, 0), Vector3(0, 0.3f, 0), Vector3(0, -1, -1.02f).unit_vect(), Vector3(0, -1, 0).unit_vect(), joint_bias);
				Cone->limit = 80;

				auto Twist = Joint::add_Twistjoint(Waist_collider, Lleg_collider, Vector3(0, 1, 0), Vector3(0, 1, 0), joint_bias);
				Twist->limit = Vector2(360 - 15, 5);
			}
			{
				auto hinge = Joint::add_Hingejoint(Lleg_collider, Lfoot_collider, Vector3(-1, -0.4f, 0), Vector3(+1, -0.3f, 0), Vector3(-1, +0.3f, 0), Vector3(+1, +0.3f, 0), joint_bias);
				hinge->limit = Vector2(0, 60);
			}

		}

		//顔とかベルトを着けてみる
		{
			auto player_material_02 = Material_manager::create_material("player_material_02");
			player_material_02->color = Vector4(sub_color, 1);
			player_material_02->Load_PS("./DefaultShader/default_ps_dither_noshadow.cso");
			{
				Gameobject* eye0 = Gameobject_manager::createSphere("eye0");
				Head->add_child(eye0);
				eye0->transform->local_pos = Vector3(+0.5f, 0.5f, -1);
				eye0->transform->local_scale = Vector3(0.25f, 0.25f, 0.25f);
				eye0->findComponent<Renderer_base>()->set_material(player_material_02);
			}
			{
				Gameobject* eye1 = Gameobject_manager::createSphere("eye1");
				Head->add_child(eye1);
				eye1->transform->local_pos = Vector3(-0.5f, 0.5f, -1);
				eye1->transform->local_scale = Vector3(0.25f, 0.25f, 0.25f);
				eye1->findComponent<Renderer_base>()->set_material(player_material_02);
			}
			{
				Gameobject* mouth = Gameobject_manager::createCube("mouth");
				Head->add_child(mouth);
				mouth->transform->local_pos = Vector3(0, -0.45f, -1);
				mouth->transform->local_scale = Vector3(0.7f, 0.25f, 0.3f);
				mouth->findComponent<Renderer_base>()->set_material(player_material_02);
			}

			{
				Gameobject* belt = Gameobject_manager::createCube("belt");
				Waist->add_child(belt);
				belt->transform->local_pos = Vector3(0, -0.45f, 0);
				belt->transform->local_scale = Vector3(1.1, 0.25f, 1.1);
				belt->findComponent<Renderer_base>()->set_material(player_material_02);
			}
		}

		{

			auto GO = Gameobject_manager::create("player");
			player = GO->addComponent<Player>();

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

			auto input_changer = GO->addComponent<Input_changer>();
			input_changer->pad_num = player_num;
		}


	}

}