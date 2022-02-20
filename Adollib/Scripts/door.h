#pragma once

#include "stageparts_base.h"
#include "../Adollib/Scripts/Main/Adollib.h"

namespace Adollib {

	namespace Stage_parts{
		// �w���tag��1�ł���Γ����M�~�b�N
		class Door : public Stageparts_base {
		public:

			Stageparts_tagbit tag = Stageparts_tags::None; //���g�̊Ǘ�����bit

			//�����ʒu
			Vector3 start_pos;
			Quaternion start_rot;

			//goal�ʒu
			Vector3 goal_pos;
			Quaternion goal_rot;

			//��������
			float pos_speed = 5;
			float rot_speed = 0;

		private:



		public:

			// addComponent���ꂽ�Ƃ��ɌĂ΂��
			void awake() override {};

			// ���t���[���Ă΂��X�V����
			void update() override{
				this_coll->ignore_tags = Collider_tags::Static_Stage;

				//if (start_pos == goal_pos)this_coll->physics_data.is_kinmatic_linear = false;
				//if (start_rot == goal_rot)this_coll->physics_data.is_kinmatic_anglar = false;
				//this_coll->physics_data.is_kinmatic_linear = false;
				//this_coll->physics_data.is_kinmatic_anglar = false;

				//�w���bit�������Ă�����͂�������
				if (this_stage->tags & tag) {

					//pos
					if (start_pos != goal_pos)
					{
						Vector3 dir = goal_pos - start_pos;
						float pow = pos_speed;

						//�s���߂��Ȃ��悤��
						if (vector3_dot(start_pos - goal_pos, transform->local_pos - goal_pos) <= 0) {
							transform->local_pos = goal_pos;
							this_coll->physics_data.is_moveable = false;
						}
						else {
							//�s���߂��Ă��Ȃ���Η͂�������
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

						//�s���߂��Ȃ��悤��
						if (vector3_dot(goal_pos - start_pos, transform->local_pos - start_pos) <= 0) {
							transform->local_pos = start_pos;
							this_coll->physics_data.is_moveable = false;
						}
						else {
							//�s���߂��Ă��Ȃ���Η͂�������
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

