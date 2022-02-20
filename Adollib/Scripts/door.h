#pragma once

#include "stageparts_base.h"
#include "../Adollib/Scripts/Main/Adollib.h"

namespace Adollib {

	namespace Stage_parts{
		// 指定のtagが1であれば動くギミック
		class Door : public Stageparts_base {
		public:

			Stageparts_tagbit tag = Stageparts_tags::None; //自身の管理するbit

			//初期位置
			Vector3 start_pos;
			Quaternion start_rot;

			//goal位置
			Vector3 goal_pos;
			Quaternion goal_rot;

			//動く速さ
			float pos_speed = 5;
			float rot_speed = 0;

		private:



		public:

			// addComponentされたときに呼ばれる
			void awake() override {};

			// 毎フレーム呼ばれる更新処理
			void update() override{
				this_coll->ignore_tags = Collider_tags::Static_Stage;

				//if (start_pos == goal_pos)this_coll->physics_data.is_kinmatic_linear = false;
				//if (start_rot == goal_rot)this_coll->physics_data.is_kinmatic_anglar = false;
				//this_coll->physics_data.is_kinmatic_linear = false;
				//this_coll->physics_data.is_kinmatic_anglar = false;

				//指定のbitが立っていたら力を加える
				if (this_stage->tags & tag) {

					//pos
					if (start_pos != goal_pos)
					{
						Vector3 dir = goal_pos - start_pos;
						float pow = pos_speed;

						//行き過ぎないように
						if (vector3_dot(start_pos - goal_pos, transform->local_pos - goal_pos) <= 0) {
							transform->local_pos = goal_pos;
							this_coll->physics_data.is_moveable = false;
						}
						else {
							//行き過ぎていなければ力を加える
							if (this_coll->gameobject->parent() != nullptr)dir = vector3_quatrotate(dir, this_coll->gameobject->parent()->transform->orientation);
							this_coll->linear_velocity(dir.unit_vect() * pos_speed);
							this_coll->physics_data.is_moveable = true;
						}
					}


					//rot
					if (start_rot != goal_rot) {}


				}
				else {
					//pos
					if (start_pos != goal_pos)
					{
						Vector3 dir = start_pos - goal_pos;
						float pow = pos_speed * 1.5f;

						//行き過ぎないように
						if (vector3_dot(goal_pos - start_pos, transform->local_pos - start_pos) <= 0) {
							transform->local_pos = start_pos;
							this_coll->physics_data.is_moveable = false;
						}
						else {
							//行き過ぎていなければ力を加える
							if (this_coll->gameobject->parent() != nullptr)dir = vector3_quatrotate(dir, this_coll->gameobject->parent()->transform->orientation);
							this_coll->linear_velocity(dir.unit_vect() * pos_speed);
							this_coll->physics_data.is_moveable = true;
						}
					}


					//rot
					if (start_rot != goal_rot) {}
				}


			};

		};


	}
}

