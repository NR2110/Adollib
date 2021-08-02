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
		pos = Vector3(-2.7f, 23, -52.2f);

		auto Go = Gameobject_manager::createSphere();
		Go->transform->local_pos = Vector3(0, 3, 0);
		debug_coll = Go->addComponent<Collider>();
		debug_coll->add_shape<Sphere>();
		debug_coll->physics_data.inertial_mass = 1;

	}

	void Player::start()
	{
		camera = Gameobject_manager::find("camera")->transform;
	}

	// 毎フレーム呼ばれる更新処理
	void Player::update()
	{
		debug_coll->add_force(Vector3(0, 30, 0));


		//Head_collider->physics_data.drag = 0;
		//Rsholder_collider->physics_data.drag = 0;
		//Relbow_collider->physics_data.drag = 0;
		//Lsholder_collider->physics_data.drag = 0;
		//Lelbow_collider->physics_data.drag = 0;
		//Body_collider->physics_data.drag = 0;
		//Waist_collider->physics_data.drag = 0;
		//Rleg_collider->physics_data.drag = 0;
		//Rfoot_collider->physics_data.drag = 0;
		//Lleg_collider->physics_data.drag = 0;
		//Lfoot_collider->physics_data.drag = 0;

		//物を持つ
		{
			const Mouse keys[2] = {
				Mouse::LBUTTON ,
				Mouse::RBUTTON };

			Collider* colliders[2] = {
				Lelbow_collider,
				Relbow_collider
			};
			Joint_base** joints[2] = {
				&catch_left_joint,
				&catch_right_joint
			};

			for (int i = 0; i < 2; i++) {
				const Mouse key = keys[i];
				auto& collider = colliders[i];
				Joint_base*& joint = *joints[i];
				//持つ
				if (input->getMouseState(key) && joint == nullptr) {
					collider->is_save_contacted_colls = true;
					auto& contacted_colls = collider->contacted_colliders;

					//衝突しているcolliderから一番近いものを探査
					Contacted_data* min_data = nullptr;
					for (auto& c_coll : contacted_colls) {
						if (min_data == nullptr || min_data->penetrate > c_coll.penetrate) {
							min_data = &c_coll;
						}
					}
					if (min_data != nullptr && min_data->coll->tag != Collider_tags::Human) {
						//Jointに登録
						joint = Joint::add_balljoint(
							collider,
							min_data->coll,
							min_data->contacted_pointA,
							min_data->contacted_pointB
						);
						joint->slop = 0.1f;
					}
				}
				//離す
				if (input->getMouseReleased(key) && joint != nullptr) {
					joint->get_colliderB()->tag &= ~Collider_tags::Having_Stage;
					joint->get_colliderB()->tag |= Collider_tags::Stage;
					Joint::delete_joint(joint);

				}

			}

			for (int i = 0; i < 2; i++) {
				Joint_base*& joint = *joints[i];
				if (joint == nullptr)continue;
				joint->get_colliderB()->tag |= Collider_tags::Having_Stage;
				joint->get_colliderB()->tag &= ~Collider_tags::Stage;
			}
		}

		//Playerが立つように
		if (!input->getKeyState(Key::LeftControl))
		{
			const float gnyat_pow = 0.9f;
			{
				//顔が赤ちゃんなのを治す
				Head_collider->physics_data.anglar_drag = 1;
				Quaternion off = Body_collider->gameobject->transform->orientation * Head_collider->gameobject->transform->orientation.inverse();
				Head_collider->add_torque(off.axis() * off.radian() * 1000 * gnyat_pow);
			}
			{
				//腰をたたせる
				Quaternion off_rot = rotate * Waist_collider->gameobject->transform->orientation.inverse();
				Waist_collider->add_torque(off_rot.axis() * off_rot.radian() * 4000 * gnyat_pow);

				//移動
				pos.y = Waist_collider->gameobject->transform->position.y;
				Vector3 off_pos = pos - Waist_collider->gameobject->transform->position;
				Waist_collider->add_force(off_pos * move_speed * gnyat_pow);

			}
			{
				//胴体をたたせる
				Quaternion off = rotate * Body_collider->gameobject->transform->orientation.inverse();
				Body_collider->add_torque(off.axis() * off.radian() * 1000 * gnyat_pow);
			}

		}
		//jointでつなげると重力が弱くなるから & 一定のパーツは重力の影響を受けないようにしているから  下向きに力を加える
		Waist_collider->add_force(Vector3(0, -1, 0) * 600);


		//Playerが歩くように
		{
			if (dir.norm() != 0) {
				float pow = 2;
				float max_theata = ToRadian(60) * 0.5f;
				//float sin = sinf(move_timer * 1.5f * DirectX::XM_2PI); //timreから足の回転量を求める
				float sin = sinf(move_timer * DirectX::XM_2PI * 0.5f); //timreから足の回転量を求める

				Vector3 waist_axis = Vector3(0, -1, 0);
				Vector3 rot_axis = Vector3(-1, 0, 0);
				//
				//Vector3 waist_axis = vector3_quatrotate(Vector3(0, -1, 0), Waist_collider->gameobject->transform->orientation);
				//Vector3 rot_axis = vector3_cross(waist_axis, dir);
				//if (waist_axis.norm() < 0)rot_axis = vector3_quatrotate(Vector3(1, 0, 0), Waist_collider->gameobject->transform->orientation);
				//rot_axis = rot_axis.unit_vect();

				Collider* collider[2]{
					Rleg_collider,
					Lleg_collider
				};
				Quaternion goal_rotate[2] = {
					quaternion_axis_radian(rot_axis, +sin * max_theata) * quaternion_axis_angle(Vector3(0,1,0),180),
					quaternion_axis_radian(rot_axis, -sin * max_theata) * quaternion_axis_angle(Vector3(0,1,0),180)
				};
				Vector3 goal_vec[2] = {
					vector3_quatrotate(waist_axis, goal_rotate[0].inverse()),
					vector3_quatrotate(waist_axis, goal_rotate[1].inverse())
				};

				Vector3 now_vec[2] = {
					vector3_quatrotate(Vector3(0, -1, 0), Rleg->transform->orientation),
					vector3_quatrotate(Vector3(0, -1, 0), Lleg->transform->orientation)
				};
				Debug::set("sin", sin);
				Debug::set("rot_axis", rot_axis);
				Debug::set("goal_vec[R]", goal_vec[0]);
				Debug::set("goal_vec[L]", goal_vec[1]);
				Debug::set("now_vec[R]", now_vec[0]);
				Debug::set("now_vec[L]", now_vec[1]);

				for (int i = 0; i < 2; i++) {
					float radian = vector3_radian(now_vec[i], goal_vec[i]);
					Vector3 axis = vector3_cross(now_vec[i], goal_vec[i]);
					if (axis.norm() < FLT_EPSILON)continue;
					axis = axis.unit_vect();

					collider[i]->add_torque(axis * radian * pow);
					//collider[i]->transform->local_orient = goal_rotate[i];

					Debug::set("axis", axis);
				}

				move_timer += Al_Global::second_per_frame;
			}
		}

		//回転
		{
			Vector3 rot_vec = Vector3(0);
			if (input->getKeyState(Key::W)) {
				rot_vec += Vector3(0, 0, -1);
			}
			if (input->getKeyState(Key::S)) {
				rot_vec += Vector3(0, 0, +1);
			}
			if (input->getKeyState(Key::A)) {
				rot_vec += Vector3(+1, 0, 0);
			}
			if (input->getKeyState(Key::D)) {
				rot_vec += Vector3(-1, 0, 0);
			}

			if (rot_vec.norm() != 0) {
				rot_vec = rot_vec.unit_vect();

				rot_vec = vector3_quatrotate(rot_vec, camera->orientation);
				rot_vec.y = 0;
				rot_vec = rot_vec.unit_vect();

				Vector3 player_vec = vector3_quatrotate(Vector3(0, 0, 1), rotate);

				float angle = vector3_angle(rot_vec, player_vec);
				if (angle > turn_speed)angle = turn_speed;

				if (vector3_cross(player_vec, rot_vec).y < 0)angle *= -1;

				rotate *= quaternion_axis_angle(Vector3(0, 1, 0), angle);

				vector3_angle(rot_vec, player_vec);

			}
		}

		//移動
		{
			Vector3 move_vec = Vector3(0);
			if (input->getKeyState(Key::W)) {
				move_vec += Vector3(0, 0, +1);
			}
			if (input->getKeyState(Key::S)) {
				move_vec += Vector3(0, 0, -1);
			}
			if (input->getKeyState(Key::A)) {
				move_vec += Vector3(-1, 0, 0);
			}
			if (input->getKeyState(Key::D)) {
				move_vec += Vector3(+1, 0, 0);
			}
			if (move_vec.norm() != 0) {
				move_vec = move_vec.unit_vect();
				move_vec = vector3_quatrotate(move_vec, camera->orientation);
				move_vec.y = 0;
				move_vec = move_vec.unit_vect();

				pos += move_vec * move_speed * Al_Global::second_per_frame;
			}
			dir = move_vec;

			if (move_vec.norm() == 0)pos = Waist_collider->gameobject->transform->position;

			float pow = 0.2f;
			if ((pos - Waist_collider->gameobject->transform->position).norm() > pow * pow)
				pos = Waist_collider->gameobject->transform->position + (pos - Waist_collider->gameobject->transform->position).unit_vect() * pow;
		}

		//ジャンプ
		{
			if (is_jumping == true)coyote += Al_Global::second_per_frame;
			if (coyote >= 0)is_jumping = false;
			if (is_jumping == false && hanger_collider->concoll_enter(Collider_tags::Stage)) coyote = 0.3f;
			if (is_jumping == false && !hanger_collider->concoll_enter(Collider_tags::Stage)) coyote -= Al_Global::second_per_frame;


			if (coyote >= 0 && input->getKeyTrigger(Key::Space)) {
				//Waist_collider->add_force(Vector3(0, 1, 0) * jump_power * Al_Global::second_per_frame);
				Waist_collider->linear_velocity(Vector3(Waist_collider->linear_velocity().x, jump_power, Waist_collider->linear_velocity().z));
				Body_collider->linear_velocity(Vector3(Waist_collider->linear_velocity().x, jump_power * 0.5f, Waist_collider->linear_velocity().z));
				is_jumping = true;
				coyote = -0.3;
			}
		}
	}

	// このスクリプトがアタッチされているGOのactiveSelfがtrueになった時呼ばれる
	void Player::onEnable()
	{

	}

	// このスクリプトがアタッチされているGOのactiveSelfがfalseになった時呼ばれる
	void Player::onDisable()
	{

	}

}
