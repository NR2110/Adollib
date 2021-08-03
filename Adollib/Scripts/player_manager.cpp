

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

		Gameobject* Rsholder = Gameobject_manager::createCube("Rsholder");
		Gameobject* Relbow = Gameobject_manager::createCube("Relbow");
		Gameobject* Lsholder = Gameobject_manager::createCube("Lsholder");
		Gameobject* Lelbow = Gameobject_manager::createCube("Lelbow");
		Gameobject* Body = Gameobject_manager::createCube("Body");
		Gameobject* Waist = Gameobject_manager::createCube("Waist");
		Gameobject* Rleg = Gameobject_manager::createCube("Rleg");
		Gameobject* Rfoot = Gameobject_manager::createCube("Rfoot");
		Gameobject* Lleg = Gameobject_manager::createCube("Lleg");
		Gameobject* Lfoot = Gameobject_manager::createCube("Lfoot");


		//::: collider,shapeのアタッチ :::
		Collider* Head_collider = Head->addComponent<Collider>();
		Collider* Rsholder_collider = Rsholder->addComponent<Collider>();
		Collider* Relbow_collider = Relbow->addComponent<Collider>();
		Collider* Lsholder_collider = Lsholder->addComponent<Collider>();
		Collider* Lelbow_collider = Lelbow->addComponent<Collider>();
		Collider* Body_collider = Body->addComponent<Collider>();
		Collider* Waist_collider = Waist->addComponent<Collider>();
		Collider* Rleg_collider = Rleg->addComponent<Collider>();
		Collider* Rfoot_collider = Rfoot->addComponent<Collider>();
		Collider* Lleg_collider = Lleg->addComponent<Collider>();
		Collider* Lfoot_collider = Lfoot->addComponent<Collider>();


		Sphere* Head_shape = Head_collider         ->add_shape<Sphere>();
		Capsule* Rsholder_shape = Rsholder_collider->add_shape<Capsule>();
		Capsule* Relbow_shape = Relbow_collider    ->add_shape<Capsule>();
		Capsule* Lsholder_shape = Lsholder_collider->add_shape<Capsule>();
		Capsule* Lelbow_shape = Lelbow_collider    ->add_shape<Capsule>();
		Box* Body_shape = Body_collider         ->add_shape<Box>();
		Box* Waist_shape = Waist_collider       ->add_shape<Box>();
		Sphere* Rleg_shape = Rleg_collider         ->add_shape<Sphere>();
		Sphere* Rfoot_shape = Rfoot_collider       ->add_shape<Sphere>();
		Sphere* Lleg_shape = Lleg_collider         ->add_shape<Sphere>();
		Sphere* Lfoot_shape = Lfoot_collider       ->add_shape<Sphere>();


		{
			//::: 親子関係の設定 :::
			Human->add_child(Body);

			Human->add_child(Head);

			Human->add_child(Rsholder);
			Human->add_child(Relbow);
			Human->add_child(Lsholder);
			Human->add_child(Lelbow);

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

			Vector3 arm_size = Vector3(0.4f, 0.5f, 0.4f);
			float arm_y_pos = 0.18f;
			{
				{
					auto& GO = Rsholder;
					GO->transform->local_scale = arm_size;
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
					auto& GO = Lsholder;
					GO->transform->local_scale = arm_size;
					GO->transform->local_orient = quaternion_from_euler(0, 0, +90);
					GO->transform->local_pos = Vector3(+(body_size.x + arm_size.x * 2), arm_y_pos, 0);
				}
				{
					auto& GO = Lelbow;
					GO->transform->local_scale = arm_size;
					GO->transform->local_orient = quaternion_from_euler(0, 0, +90);
					GO->transform->local_pos = Vector3(+(body_size.x + arm_size.x * 5), arm_y_pos, 0);
				}
			}

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

			//::: 質量の調整 :::
			Head_collider    ->physics_data.inertial_mass = 3;
			Rsholder_collider->physics_data.inertial_mass = 2;
			Relbow_collider  ->physics_data.inertial_mass = 1;
			Lsholder_collider->physics_data.inertial_mass = 2;
			Lelbow_collider  ->physics_data.inertial_mass = 1;
			Body_collider    ->physics_data.inertial_mass = 5;
			Waist_collider   ->physics_data.inertial_mass = 4;
			Rleg_collider    ->physics_data.inertial_mass = 2;
			Rfoot_collider   ->physics_data.inertial_mass = 1;
			Lleg_collider    ->physics_data.inertial_mass = 2;
			Lfoot_collider   ->physics_data.inertial_mass = 1;

			Rleg_collider->physics_data.anglar_drag = 0.95f;
			Lleg_collider->physics_data.anglar_drag = 0.95f;

			//::: 重力適用の調整 :::
			Head_collider->physics_data.is_fallable = false;
			Rsholder_collider->physics_data.is_fallable  = false;
			Lsholder_collider->physics_data.is_fallable  = false;
			Rleg_collider->physics_data.is_fallable = false;
			Lleg_collider->physics_data.is_fallable = false;

			//::: tagの設定 :::
			Head_collider    ->tag = Collider_tags::Human;
			Rsholder_collider->tag = Collider_tags::Human;
			Relbow_collider  ->tag = Collider_tags::Human;
			Lsholder_collider->tag = Collider_tags::Human;
			Lelbow_collider  ->tag = Collider_tags::Human;
			Body_collider    ->tag = Collider_tags::Human;
			Waist_collider   ->tag = Collider_tags::Human;
			Rleg_collider    ->tag = Collider_tags::Human;
			Rfoot_collider   ->tag = Collider_tags::Human;
			Lleg_collider    ->tag = Collider_tags::Human;
			Lfoot_collider   ->tag = Collider_tags::Human;


			//::: capsuleの調整 :::
			Relbow_shape->length *= (Relbow->transform->local_scale.y - Relbow->transform->local_scale.x) / (Relbow->transform->local_scale.y);
			Rsholder_shape->length *= (Rsholder->transform->local_scale.y - Rsholder->transform->local_scale.x) / (Rsholder->transform->local_scale.y);
			Lelbow_shape->length *= (Lelbow->transform->local_scale.y - Lelbow->transform->local_scale.x) / (Lelbow->transform->local_scale.y);
			Lsholder_shape->length *= (Lsholder->transform->local_scale.y - Lsholder->transform->local_scale.x) / (Lsholder->transform->local_scale.y);

			//Body_shape->length *= (Body->transform->local_scale.y - Body->transform->local_scale.x) / (Body->transform->local_scale.y);

			//Rleg_shape->length *= Rleg->transform->local_scale.y / (Rleg->transform->local_scale.x + Rleg->transform->local_scale.y);
			//Rfoot_shape->length *= Rfoot->transform->local_scale.y / (Rfoot->transform->local_scale.x + Rfoot->transform->local_scale.y);
			//Lleg_shape->length *= Lleg->transform->local_scale.y / (Lleg->transform->local_scale.x + Lleg->transform->local_scale.y);
			//Lfoot_shape->length *= Lfoot->transform->local_scale.y / (Lfoot->transform->local_scale.x + Lfoot->transform->local_scale.y);

			//::: sphereの調整 :::
			Rleg_shape->r = Rleg->transform->local_scale.y * 2;
			Rfoot_shape->r = Rfoot->transform->local_scale.y * 2;
			Lleg_shape->r = Lleg->transform->local_scale.y * 2;
			Lfoot_shape->r = Lfoot->transform->local_scale.y * 2;

			//Body_shape->r = Body->transform->local_scale.y ;
			//Waist_shape->r = Waist->transform->local_scale.y ;




			//::: Jointの設定
			//頭
			{
				auto Ball = Joint::add_Conejoint(Head_collider, Body_collider, Vector3(0, -1, 0), Vector3(0, 0.8f, 0), Vector3(0, 1, 0), Vector3(0, 1, 0));
				Ball->limit = 30;

				auto Twist = Joint::add_Twistjoint(Body_collider, Head_collider, Vector3(0, 1, 0));
				Twist->limit = Vector2(360 - 90, 90);
			}
			//腕
			{
				auto Cone = Joint::add_Conejoint(Body_collider, Rsholder_collider, Vector3(-1.1f, 0.2f, 0), Vector3(0, 0.5f, 0), Vector3(-1, 0, -1).unit_vect(), Vector3(0, -1, 0).unit_vect());
				Cone->limit = 85;

				auto Twist = Joint::add_Twistjoint(Body_collider, Rsholder_collider, Vector3(0, 1, 0));
				Twist->limit = Vector2(360 - 90, 90);
			}
			{
				auto hinge = Joint::add_Hingejoint(Rsholder_collider, Relbow_collider, Vector3(-1, -0.6f, 0), Vector3(+1, -0.6f, 0), Vector3(-1, 0.6f, 0), Vector3(+1, 0.6f, 0));
				hinge->limit = Vector2(230, 360);
			}
			{
				auto Cone = Joint::add_Conejoint(Body_collider, Lsholder_collider, Vector3(+1.1f, 0.2f, 0), Vector3(0, 0.5f, 0), Vector3(-1, 0, -1).unit_vect(), Vector3(0, -1, 0).unit_vect());
				Cone->limit = 85;

				auto Twist = Joint::add_Twistjoint(Body_collider, Lsholder_collider, Vector3(0, 1, 0));
				Twist->limit = Vector2(360 - 90, 90);
			}
			{
				auto hinge = Joint::add_Hingejoint(Lsholder_collider, Lelbow_collider, Vector3(-1, -0.6f, 0), Vector3(+1, -0.6f, 0), Vector3(-1, 0.6f, 0), Vector3(+1, 0.6f, 0));
				hinge->limit = Vector2(230, 360);
			}
			//腰
			{
				auto Cone = Joint::add_Conejoint(Body_collider, Waist_collider, Vector3(0, -1.0f, 0), Vector3(0, 0.5f, 0), Vector3(0, 1, 0).unit_vect(), Vector3(0, 1.5f, -1).unit_vect());
				Cone->limit = 60;

				auto Twist = Joint::add_Twistjoint(Body_collider, Waist_collider, Vector3(0, 1, 0));
				Twist->limit = Vector2(360 - 30, 30);
			}
			//足
			{
				auto Cone = Joint::add_Conejoint(Waist_collider, Rleg_collider, Vector3(-0.6f, -0.8, 0), Vector3(0, 0.3, 0), Vector3(0, -1, -1.02f).unit_vect(), Vector3(0, -1, 0).unit_vect());
				//Cone->limit = 48;
				Cone->limit = 60;

				auto Twist = Joint::add_Twistjoint(Waist_collider, Rleg_collider, Vector3(0, 1, 0));
				Twist->limit = Vector2(360 - 5, 15);
			}
			{
				auto hinge = Joint::add_Hingejoint(Rleg_collider, Rfoot_collider, Vector3(-1, -0.4f, 0), Vector3(+1, -0.3f, 0), Vector3(-1, +0.3f, 0), Vector3(+1, +0.3f, 0));
				hinge->limit = Vector2(0, 60);
			}
			{
				auto Cone = Joint::add_Conejoint(Waist_collider, Lleg_collider, Vector3(+0.6f, -0.8, 0), Vector3(0, 0.3, 0), Vector3(0, -1, -1.02f).unit_vect(), Vector3(0, -1, 0).unit_vect());
				//Cone->limit = 48;
				Cone->limit = 60;

				auto Twist = Joint::add_Twistjoint(Waist_collider, Lleg_collider, Vector3(0, 1, 0));
				Twist->limit = Vector2(360 - 15, 5);
			}
			{
				auto hinge = Joint::add_Hingejoint(Lleg_collider, Lfoot_collider, Vector3(-1, -0.4f, 0), Vector3(+1, -0.3f, 0), Vector3(-1, +0.3f, 0), Vector3(+1, +0.3f, 0));
				hinge->limit = Vector2(0, 60);
			}
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

		//onground用のcolliderのアタッチ
		auto Waist_capsule = Gameobject_manager::create("Waist_capsule");
		auto Waist_capsule_collider = Waist_capsule->addComponent<Collider>();
		{
			auto Waist_capsule_shape = Waist_capsule_collider->add_shape<Capsule>();
			Human->add_child(Waist_capsule);

			Waist_capsule_shape->r = 0.15f;
			Waist_capsule_shape->length = 0.68f;
			Waist_capsule_shape->center = Vector3(0, -1.15, 0);

			Waist_capsule_collider->ignore_tags = Collider_tags::Human;
			//Waist_capsule_collider->physics_data.is_hitable = false;



			//Waist_capsule_collider->physics_data.is_kinmatic_anglar = false;

			auto hinge = Joint::add_Hingejoint(Waist_collider, Waist_capsule_collider, Vector3(0, 1, 0), Vector3(0, -1, 0), Vector3(0, 1, 0), Vector3(0, -1, 0));
			hinge->hinge_pow = 0;
			//auto ball = Joint::add_balljoint(Waist_capsule_collider, Head_collider, Vector3(0, 3.2, 0), Vector3(0, 0, 0), 0.01f);
		}

		{

			auto GO = Gameobject_manager::create("player");
			player = GO->addComponent<Player>();

			player->set_player_colliders(
				Head_collider,
				Rsholder_collider,
				Relbow_collider,
				Lsholder_collider,
				Lelbow_collider,
				Body_collider,
				Waist_collider,
				Rleg_collider,
				Rfoot_collider,
				Lleg_collider,
				Lfoot_collider,
				Waist_capsule_collider
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