#pragma once

#include "stageparts_base.h"
#include "../Adollib/Scripts/Main/Adollib.h"

namespace Adollib {

	namespace Stage_parts {
		// �w���tag��1�ł���Γ����M�~�b�N
		class Move_block_from_2flags : public Stageparts_base {
		public:
			Stageparts_tagbit left_tag = Stageparts_tags::None; //���g�̊Ǘ�����bit
			Stageparts_tagbit right_tag = Stageparts_tags::None; //���g�̊Ǘ�����bit

			// goal�ʒu
			Vector3 left_pos;
			Vector3 right_pos;

			// �[����[�ɓ��B���鎞�Ԃ�inverse
			float time_pow = 1;

			// ���݂̎���
			float now_time = 0.5f;

		private:
			float acc = 0;


		public:

			// addComponent���ꂽ�Ƃ��ɌĂ΂��
			void awake() override {};

			// ���t���[���Ă΂��X�V����
			void update() override {

				//float add_time = 0;
				//if (this_stage->tags & left_tag) add_time += time->deltaTime() * time_pow * acc;
				//if (this_stage->tags & right_tag)add_time -= time->deltaTime() * time_pow * acc;

				//now_time += add_time;
				//now_time = ALClamp(now_time, 0, 1);

				//if (add_time == 0) acc = 0;
				//acc += time->deltaTime() * 2;
				//acc = ALClamp(acc, 0, 1);


				//Vector3 offset = left_pos + (right_pos - left_pos) * now_time - transform->local_pos;
				//transform->local_pos += offset;

				const Vector3 left_to_right_dir = (right_pos - left_pos).unit_vect();

				// ���W���� ���[�g���O��Ă����ꍇ ���[�g��ɋ����I�ɖ߂�
				transform->local_pos = left_pos + left_to_right_dir * vector3_dot(left_to_right_dir, transform->local_pos - left_pos);
				// ���W���� �͂ݏo�Ă�����߂�
				if (vector3_dot(left_pos - transform->local_pos, right_pos - transform->local_pos) > 0) {
					if ((left_pos - transform->local_pos).norm() < (right_pos - transform->local_pos).norm())
						transform->local_pos = left_pos;
					else
						transform->local_pos = right_pos;
				}

				// ���x�̕���
				float sign = 0;
				if (this_stage->tags & left_tag) sign -= 1;
				if (this_stage->tags & right_tag)sign += 1;

				// �ڕW�n�_�ɂ��ǂ蒅���Ă���� ���x��0�ɂ���return
				if (
					(sign == -1 && transform->local_pos == left_pos) ||
					(sign == +1 && transform->local_pos == right_pos)
					) {
					this_coll->linear_velocity(Vector3(0));
					acc = 0;
				}

				// �K����easing�p
				if (sign == 0)acc -= time->deltaTime();
				else acc += time->deltaTime() * 2;
				acc = ALClamp(acc, 0, 1);

				Vector3 vec = left_to_right_dir * sign * acc;

				this_coll->linear_velocity(vec * time_pow * 60);

			};

		};


	}
}

