#pragma once

#include "stageparts_base.h"
#include "../Adollib/Scripts/Main/Adollib.h"

namespace Adollib {

	namespace Stage_parts {
		// �w���tag��1�ł���Γ����M�~�b�N
		class Rotate_block : public Stageparts_base {
		public:
			Stageparts_tagbit tag = Stageparts_tags::None; //���g�̊Ǘ�����bit

			Vector3 rotate_axis = Vector3(1, 0, 0); //��]��
			float rotate_speed = 0.1f; //��]���x
			float rotate_speed_easing_pow = 1; //��]���x��easing�̒l 0����rotate_speed�܂ŁA���̎��Ԃœ��B���� easing�� y = x^2


		private:

			float timer = 0;



		public:

			// addComponent���ꂽ�Ƃ��ɌĂ΂��
			void awake() override {};

			// ���t���[���Ă΂��X�V����
			void update() override {

				// tag�������Ă�����timer��i�߂�
				if (this_stage->tags & tag) timer += time->deltaTime() / rotate_speed_easing_pow;
				else timer -= time->deltaTime() / rotate_speed_easing_pow;
				timer = ALClamp(timer, 0, 1);

				// timer�̒l�����]��������߂�
				const float& speed = (timer * timer) * rotate_speed;
				if (speed != 0) {
					this_coll->angula_velocity(rotate_axis * speed);
				}
					//transform->local_orient *= quaternion_axis_angle(rotate_axis, speed);

			};

		};


	}
}

