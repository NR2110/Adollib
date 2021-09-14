

#include "player_manager.h"
#include "../Adollib/Scripts/Object/gameobject_manager.h"

#include "../Adollib/Scripts/Imgui/imgui_all.h"

#include "../Adollib/Scripts/Physics/ALP__physics_manager.h"
#include "../Adollib/Scripts/Physics/collider.h"
#include "../Adollib/Scripts/Physics/joint.h"

#include "../Adollib/Scripts/Imgui/debug.h"

#include "player.h"

namespace Adollib
{
	// 所属するシーンの初期化時に一度だけ呼ばれる
	void Player_manager::awake() {

		Vector3 player_start_pos = Vector3(-2.7f, 23, -52.2f);
		Quaternion player_start_rotate = quaternion_from_euler(0, 180, 0);

		Gameobject* Human = Gameobject_manager::create("Human");
		Human->transform->local_pos = player_start_pos;
		Human->transform->local_orient = player_start_rotate;

		//::: Gameobjectの生成 :::
		Gameobject* Head = Gameobject_manager::createCube("Head");

		Gameobject* Lsholder = Gameobject_manager::createCube("Lsholder");
		Gameobject* Lelbow = Gameobject_manager::createCube("Lelbow");
		Gameobject* Lhand = Gameobject_manager::create("Lhand");
		Gameobject* Rsholder = Gameobject_manager::createCube("Rsholder");
		Gameobject* Relbow = Gameobject_manager::createCube("Relbow");
		Gameobject* Rhand = Gameobject_manager::create("Rhand");
		Gameobject* Body = Gameobject_manager::createCube("Body");
		Gameobject* Waist = Gameobject_manager::createCube("Waist");
		Gameobject* Rleg = Gameobject_manager::createCube("Rleg");
		Gameobject* Rfoot = Gameobject_manager::createCube("Rfoot");
		Gameobject* Lleg = Gameobject_manager::createCube("Lleg");
		Gameobject* Lfoot = Gameobject_manager::createCube("Lfoot");


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
		Sphere* Rleg_shape = Rleg_collider->add_shape<Sphere>();
		Sphere* Rfoot_shape = Rfoot_collider->add_shape<Sphere>();
		Sphere* Lleg_shape = Lleg_collider->add_shape<Sphere>();
		Sphere* Lfoot_shape = Lfoot_collider->add_shape<Sphere>();


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

			Vector3 sholder_size = Vector3(0.4f, 0.5f, 0.4f);
			Vector3 arm_size = Vector3(0.4f, 0.5f, 0.4f);
			Vector3 hand_size = Vector3(0.45f, 0.45f, 0.45f);
			float arm_y_pos = 0.18f;
			{
				{
					auto& GO = Lsholder;
					GO->transform->local_scale = sholder_size;
					GO->transform->local_orient = quaternion_from_euler(0, 0, +90);
					GO->transform->local_pos = Vector3(+(body_size.x + arm_size.x * 2), arm_y_pos, 0);
				}
				{
					auto& GO = Lelbow;
					GO->transform->local_scale = arm_size;
					GO->transform->local_orient = quaternion_from_euler(0, 0, +90);
					GO->transform->local_pos = Vector3(+(body_size.x + arm_size.x * 5), arm_y_pos, 0);
				}
				{
					auto& GO = Lhand;
					GO->transform->local_scale = hand_size;
					GO->transform->local_orient = quaternion_from_euler(0, 0, 0);
					GO->transform->local_pos = Vector3(+(body_size.x + arm_size.x * 6), arm_y_pos, 0);
				}
				{
					auto& GO = Rsholder;
					GO->transform->local_scale = sholder_size;
					GO->transform->local_orient = quaternion_from_euler(0, 0, -90);
					GO->transform->local_pos = Vector3(-(body_size.x + arm_size.x * 2), arm_y_pos, 0);
				}
				{
					auto& GO = Relbow;
					GO->transform->local_scale = arm_size;
					GO->transform->local_orient = quaternion_from_euler(0, 0, -90);
					GO->transform->local_pos = Vector3(-(body_size.x + arm_size.x * 5), arm_y_pos, 0);
				}
				{
					auto& GO = Rhand;
					GO->transform->local_scale = hand_size;
					GO->transform->local_orient = quaternion_from_euler(0, 0, 0);
					GO->transform->local_pos = Vector3(-(body_size.x + arm_size.x * 6), arm_y_pos, 0);
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
			Head_collider	 ->set_barycenter(Vector3(0, +0.5f, 0));
			Body_collider	 ->set_barycenter(Vector3(0, -0.2f, 0));

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
			//Rsholder_collider->set_tensor(Rsholder_collider->get_tensor());


			//Relbow_collider  ->set_tensor(Head_collider->get_tensor());
			//Lsholder_collider->set_tensor(Head_collider->get_tensor());
			//Lelbow_collider  ->set_tensor(Head_collider->get_tensor());
			//Body_collider    ->set_tensor(Head_collider->get_tensor());
			//Waist_collider   ->set_tensor(Head_collider->get_tensor());
			//Rleg_collider    ->set_tensor(Head_collider->get_tensor());
			//Rfoot_collider   ->set_tensor(Head_collider->get_tensor());
			//Lleg_collider    ->set_tensor(Head_collider->get_tensor());
			//Lfoot_collider   ->set_tensor(Head_collider->get_tensor());


			//::: 質量の調整 :::
			Head_collider->physics_data.inertial_mass = 3;
			Lsholder_collider->physics_data.inertial_mass = 3;
			Rsholder_collider->physics_data.inertial_mass = 3;
			Lelbow_collider->physics_data.inertial_mass = 4;
			Relbow_collider->physics_data.inertial_mass = 4;
			Lhand_collider->physics_data.inertial_mass = 1;
			Rhand_collider->physics_data.inertial_mass = 1;
			Body_collider->physics_data.inertial_mass = 5;
			Waist_collider->physics_data.inertial_mass = 4;
			Lleg_collider->physics_data.inertial_mass = 2;
			Rleg_collider->physics_data.inertial_mass = 2;
			Lfoot_collider->physics_data.inertial_mass = 1;
			Rfoot_collider->physics_data.inertial_mass = 1;

			//Lsholder_collider->physics_data.inertial_mass = 3;
			//Rsholder_collider->physics_data.inertial_mass = 3;
			//Lelbow_collider->physics_data.inertial_mass = 4;
			//Relbow_collider->physics_data.inertial_mass = 4;
			//Lhand_collider->physics_data.inertial_mass = 4;
			//Rhand_collider->physics_data.inertial_mass = 4;
			//Head_collider    ->physics_data.inertial_mass = 6;
			//Lsholder_collider->physics_data.inertial_mass = 6;
			//Lelbow_collider  ->physics_data.inertial_mass = 4;
			//Lhand_collider   ->physics_data.inertial_mass = 2;
			//Rsholder_collider->physics_data.inertial_mass = 6;
			//Relbow_collider  ->physics_data.inertial_mass = 4;
			//Rhand_collider   ->physics_data.inertial_mass = 2;
			//Body_collider    ->physics_data.inertial_mass = 10;
			//Waist_collider   ->physics_data.inertial_mass = 8;
			//Rleg_collider    ->physics_data.inertial_mass = 4;
			//Rfoot_collider   ->physics_data.inertial_mass = 2;
			//Lleg_collider    ->physics_data.inertial_mass = 4;
			//Lfoot_collider   ->physics_data.inertial_mass = 2;
			//Head_collider    ->physics_data.inertial_mass = 1;
			//Lsholder_collider->physics_data.inertial_mass = 1;
			//Lelbow_collider  ->physics_data.inertial_mass = 1;
			//Lhand_collider   ->physics_data.inertial_mass = 1;
			//Rsholder_collider->physics_data.inertial_mass = 1;
			//Relbow_collider  ->physics_data.inertial_mass = 1;
			//Rhand_collider   ->physics_data.inertial_mass = 1;
			//Body_collider    ->physics_data.inertial_mass = 1;
			//Waist_collider   ->physics_data.inertial_mass = 1;
			//Rleg_collider    ->physics_data.inertial_mass = 1;
			//Rfoot_collider   ->physics_data.inertial_mass = 1;
			//Lleg_collider    ->physics_data.inertial_mass = 1;
			//Lfoot_collider   ->physics_data.inertial_mass = 1;


			Head_collider->physics_data.anglar_drag = 0.5f;
			Lsholder_collider->physics_data.anglar_drag = 0.5f;
			Lelbow_collider->physics_data.anglar_drag = 0.5f;
			Lhand_collider->physics_data.anglar_drag = 0.5f;
			Rsholder_collider->physics_data.anglar_drag = 0.5f;
			Relbow_collider->physics_data.anglar_drag = 0.5f;
			Rhand_collider->physics_data.anglar_drag = 0.5f;
			Body_collider->physics_data.anglar_drag = 0.9f;
			Waist_collider->physics_data.anglar_drag = 0.8f;
			Rleg_collider->physics_data.anglar_drag = 0.5f;
			Rfoot_collider->physics_data.anglar_drag = 0.5f;
			Lleg_collider->physics_data.anglar_drag = 0.5f;
			Lfoot_collider->physics_data.anglar_drag = 0.5f;

			Rleg_collider->physics_data.anglar_drag = 0.95f;
			Lleg_collider->physics_data.anglar_drag = 0.95f;

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

			//::: capsuleの調整 :::
			//Lelbow_shape->length *= (Lelbow->transform->local_scale.y - Lelbow->transform->local_scale.x) / (Lelbow->transform->local_scale.y);
			//Lsholder_shape->length *= (Lsholder->transform->local_scale.y - Lsholder->transform->local_scale.x) / (Lsholder->transform->local_scale.y);
			//Relbow_shape->length *= (Relbow->transform->local_scale.y - Relbow->transform->local_scale.x) / (Relbow->transform->local_scale.y);
			//Rsholder_shape->length *= (Rsholder->transform->local_scale.y - Rsholder->transform->local_scale.x) / (Rsholder->transform->local_scale.y);

			//Body_shape->length *= (Body->transform->local_scale.y - Body->transform->local_scale.x) / (Body->transform->local_scale.y);

			//Rleg_shape->length *= Rleg->transform->local_scale.y / (Rleg->transform->local_scale.x + Rleg->transform->local_scale.y);
			//Rfoot_shape->length *= Rfoot->transform->local_scale.y / (Rfoot->transform->local_scale.x + Rfoot->transform->local_scale.y);
			//Lleg_shape->length *= Lleg->transform->local_scale.y / (Lleg->transform->local_scale.x + Lleg->transform->local_scale.y);
			//Lfoot_shape->length *= Lfoot->transform->local_scale.y / (Lfoot->transform->local_scale.x + Lfoot->transform->local_scale.y);

			//::: sphereの調整 :::
			Lleg_shape->r = Lleg->transform->local_scale.y * 2;
			Lfoot_shape->r = Lfoot->transform->local_scale.y * 2;
			Rleg_shape->r = Rleg->transform->local_scale.y * 2;
			Rfoot_shape->r = Rfoot->transform->local_scale.y * 2;

			//Body_shape->r = Body->transform->local_scale.y;
			//Waist_shape->r = Waist->transform->local_scale.y;


			//auto a = Body_collider->get_tensor();



			//::: Jointの設定
			float joint_bias = 0.1f;
			//頭
			{
				auto Ball = Joint::add_Conejoint(Head_collider, Body_collider, Vector3(0, -1, 0), Vector3(0, 0.8f, 0), Vector3(0, 1, 0), Vector3(0, 1, 0), joint_bias);
				Ball->limit = 30;

				auto Twist = Joint::add_Twistjoint(Body_collider, Head_collider, Vector3(0, 1, 0), joint_bias);
				Twist->limit = Vector2(360 - 90, 90);
			}
			//腕
			{
				auto Cone = Joint::add_Conejoint(Body_collider, Lsholder_collider, Vector3(+1.1f, 0.2f, 0), Vector3(0, 0.5f, 0), Vector3(-1, 0, -1).unit_vect(), Vector3(0, -1, 0).unit_vect(), joint_bias);
				Cone->limit = 85;

				auto Twist = Joint::add_Twistjoint(Body_collider, Lsholder_collider, Vector3(0, 1, 0), joint_bias);
				Twist->limit = Vector2(360 - 90, 90);
			}
			{
				auto hinge = Joint::add_Hingejoint(Lsholder_collider, Lelbow_collider, Vector3(-1, -0.6f, 0), Vector3(+1, -0.6f, 0), Vector3(-1, 0.6f, 0), Vector3(+1, 0.6f, 0), joint_bias);
				hinge->bias = 0.5f;
				hinge->limit = Vector2(230, 360);

				//auto ball = Joint::add_balljoint(Lsholder_collider, Lelbow_collider, Vector3(0, -0.9f, 0), Vector3(0, 0.6f, 0), joint_bias);
			}
			{
				auto ball = Joint::add_balljoint(Lelbow_collider, Lhand_collider, Vector3(-0.2f, -0.25f, 0), Vector3(0, 0, 0), joint_bias);
			}
			{
				auto Cone = Joint::add_Conejoint(Body_collider, Rsholder_collider, Vector3(-1.1f, 0.2f, 0), Vector3(0, 0.5f, 0), Vector3(+1, 0, -1).unit_vect(), Vector3(0, -1, 0).unit_vect(), joint_bias);
				Cone->limit = 85;

				auto Twist = Joint::add_Twistjoint(Body_collider, Rsholder_collider, Vector3(0, 1, 0), joint_bias);
				Twist->limit = Vector2(360 - 90, 90);
			}
			{
				auto hinge = Joint::add_Hingejoint(Rsholder_collider, Relbow_collider, Vector3(-1, -0.6f, 0), Vector3(+1, -0.6f, 0), Vector3(-1, 0.6f, 0), Vector3(+1, 0.6f, 0), joint_bias);
				hinge->bias = 0.5f;
				hinge->limit = Vector2(230, 360);

				//auto ball = Joint::add_balljoint(Rsholder_collider, Relbow_collider, Vector3(0, -0.9f, 0), Vector3(0, 0.6f, 0), joint_bias);
			}
			{
				auto ball = Joint::add_balljoint(Relbow_collider, Rhand_collider, Vector3(0.2f, -0.25f, 0), Vector3(0, 0, 0), joint_bias);
			}
			//腰
			{
				auto Cone = Joint::add_Conejoint(Body_collider, Waist_collider, Vector3(0, -1.0f, 0), Vector3(0, 0.5f, 0), Vector3(0, 1, 0).unit_vect(), Vector3(0, 1.5f, -1).unit_vect(),0.1f);
				Cone->limit = 60;

				auto Twist = Joint::add_Twistjoint(Body_collider, Waist_collider, Vector3(0, 1, 0), joint_bias);
				Twist->limit = Vector2(360 - 30, 30);

				//auto ball = Joint::add_balljoint(Body_collider, Waist_collider, Vector3(0, -1.0f, 0), Vector3(0, 0.5f, 0));
			}
			//足
			{
				auto Cone = Joint::add_Conejoint(Waist_collider, Rleg_collider, Vector3(-0.6f, -0.8f, 0), Vector3(0, 0.3, 0), Vector3(0, -1, -1.02f).unit_vect(), Vector3(0, -1, 0).unit_vect(), joint_bias);
				//Cone->limit = 48;
				Cone->limit = 80;

				auto Twist = Joint::add_Twistjoint(Waist_collider, Rleg_collider, Vector3(0, 1, 0), joint_bias);
				Twist->limit = Vector2(360 - 5, 15);
			}
			{
				auto hinge = Joint::add_Hingejoint(Rleg_collider, Rfoot_collider, Vector3(-1, -0.4f, 0), Vector3(+1, -0.3f, 0), Vector3(-1, +0.3f, 0), Vector3(+1, +0.3f, 0), joint_bias);
				//hinge->hinge_pow = 0.01f;
				hinge->limit = Vector2(0, 60);
			}
			{
				auto Cone = Joint::add_Conejoint(Waist_collider, Lleg_collider, Vector3(+0.6f, -0.8f, 0), Vector3(0, 0.3f, 0), Vector3(0, -1, -1.02f).unit_vect(), Vector3(0, -1, 0).unit_vect(), joint_bias);
				//Cone->limit = 48;
				Cone->limit = 80;

				auto Twist = Joint::add_Twistjoint(Waist_collider, Lleg_collider, Vector3(0, 1, 0), joint_bias);
				Twist->limit = Vector2(360 - 15, 5);
			}
			{
				auto hinge = Joint::add_Hingejoint(Lleg_collider, Lfoot_collider, Vector3(-1, -0.4f, 0), Vector3(+1, -0.3f, 0), Vector3(-1, +0.3f, 0), Vector3(+1, +0.3f, 0), joint_bias);
				//hinge->hinge_pow = 0.01f;
				hinge->limit = Vector2(0, 60);
			}
		}

		//関節をつなげるGO,shape,jointの生成
		{

			//auto connect_parts = Gameobject_manager::create("connect_parts");
			//Human->add_child(connect_parts);

			//Physics_data defult_data;
			//{
			//	auto Go = Gameobject_manager::create("Body_Waist");
			//	Human->add_child(Go);

			//	auto collider = Go->addComponent<Collider>();
			//	collider->ignore_tags |= Collider_tags::Human;
			//	collider->tag |= Collider_tags::Human;

			//	defult_data = collider->physics_data; //physics_dataの初期値をここで決める

			//	auto waist_body = collider->add_shape<Capsule>();
			//	waist_body->r = Waist_shape->r * Waist->transform->local_scale.x;
			//	waist_body->length = 1.5f * 0.5f;

			//	Joint::add_balljoint(Body_collider , collider, Vector3(0, 0, 0), Vector3(0, +0.75f, 0));
			//	Joint::add_balljoint(Waist_collider, collider, Vector3(0, 0, 0), Vector3(0, -0.75f, 0));
			//}



		}

		//顔とかベルトを着けてみる
		{
			Vector4 face_color = Vector4(0.5f, 0.5f, 0.5f, 1);
			{
				Gameobject* eye0 = Gameobject_manager::createSphere("eye0");
				Head->add_child(eye0);
				eye0->transform->local_pos = Vector3(+0.5f, 0.5f, -1);
				eye0->transform->local_scale = Vector3(0.25f, 0.25f, 0.25f);
				eye0->material->color = face_color;
			}
			{
				Gameobject* eye1 = Gameobject_manager::createSphere("eye1");
				Head->add_child(eye1);
				eye1->transform->local_pos = Vector3(-0.5f, 0.5f, -1);
				eye1->transform->local_scale = Vector3(0.25f, 0.25f, 0.25f);
				eye1->material->color = face_color;
			}
			{
				Gameobject* mouth = Gameobject_manager::createCube("mouth");
				Head->add_child(mouth);
				mouth->transform->local_pos = Vector3(0, -0.45f, -1);
				mouth->transform->local_scale = Vector3(0.7f, 0.25f, 0.3f);
				mouth->material->color = face_color;
			}

			{
				Gameobject* belt = Gameobject_manager::createCube("belt");
				Waist->add_child(belt);
				belt->transform->local_pos = Vector3(0, -0.45f, 0);
				belt->transform->local_scale = Vector3(1.1, 0.25f, 1.1);
				belt->material->color = face_color;
			}
		}

		//onground用のcolliderのアタッチ 上でアタッチしたsphereと同じ位置へ
		//Collider* Waist_sphere_collider = nullptr;
		//BallJoint* waist_sphere_joint = nullptr;
		//{
		//	auto Waist_sphere = Gameobject_manager::create("Waist_sphere");
		//	Human->add_child(Waist_sphere);
		//	Waist_sphere->transform->local_scale = Vector3(0.4f);
		//	Waist_sphere->transform->local_pos = Vector3(0, -2.5f, 0);

		//	Waist_sphere_collider = Waist_sphere->addComponent<Collider>();

		//	Waist_sphere_collider->physics_data.inertial_mass = 10;
		//	Waist_sphere_collider->physics_data.is_fallable = false;
		//	Waist_sphere_collider->physics_data.is_kinmatic_anglar = false;
		//	Waist_sphere_collider->ignore_tags |= Collider_tags::Human;


		//	auto Waist_sphere_shape = Waist_sphere_collider->add_shape<Sphere>();

		//	waist_sphere_joint = Joint::add_balljoint(Waist_collider, Waist_sphere_collider, Vector3(0, -1.8f, 0), Vector3(0), 0.1f);
		//}
		//auto Waist_sphere = Gameobject_manager::create("Waist_sphere");
		auto Waist_sphere = Waist_collider->add_shape<Sphere>();
		{

			Waist_sphere->r = 6.f;
			Waist_sphere->center = Vector3(0, -2.8f, 0);
			Waist_collider->set_tensor(Waist_collider->get_tensor());

			Waist_sphere->center = Vector3(0, -2.6f, 0);
			Waist_sphere->r = 0.25f;
			Waist_collider->physics_data.restitution = 0;
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
				Lfoot_collider,
				Waist_sphere
			);

			//auto shape = Waist_collider->add_shape<Capsule>();
			//shape->r = 0.15f;
			//shape->length = 1.4f;
			//shape->center = Vector3(0, -1.15, 0);
		}

	}

	void Player_manager::start()
	{

	}

	// 毎フレーム呼ばれる更新処理
	void Player_manager::update()
	{

	}




	// このスクリプトがアタッチされているGOのactiveSelfがtrueになった時呼ばれる
	void Player_manager::onEnable()
	{

	}

	// このスクリプトがアタッチされているGOのactiveSelfがfalseになった時呼ばれる
	void Player_manager::onDisable()
	{

	}

}