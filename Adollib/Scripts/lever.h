#pragma once

// �{�^�� ������stage_base�̕ϐ��̎w���bit������

#include "stageparts_base.h"
#include "../Adollib/Scripts/Main/Adollib.h"

namespace Adollib {

	namespace Stage_parts {

#pragma optimize("", off)
		class Lever : public Stageparts_base {
		public:
			Stageparts_tagbit left_tag = Stageparts_tags::None; //���g�̊Ǘ�����bit
			Stageparts_tagbit right_tag = Stageparts_tags::None; //���g�̊Ǘ�����bit

			Vector3 base_dir = Vector3(0, 1, 0);
			Vector3 hinge_dir = Vector3(1, 0, 0);

		private:

		public:

			// addComponent���ꂽ�Ƃ��ɌĂ΂��
			void awake() override {};

			// ���t���[���Ă΂��X�V����
			void update() override {

				// bit�̏�����
				this_stage->tags &= ~left_tag;
				this_stage->tags &= ~right_tag;

				// ���݂�lever�̌���
				Vector3 now_dir = vector3_quatrotate(base_dir, transform->orientation);

				// ��]��
				Vector3 rot_axis = vector3_cross(now_dir, base_dir);
				this_coll->add_torque(rot_axis * fabsf(transform->orientation.radian() * 1000));

				// lever�����܂�|��Ă��Ȃ����flag�𗧂Ă���return
				if (fabsf(vector3_dot(base_dir, now_dir)) > 0.96f)return;


				Vector3 roted_hinge_dir = vector3_quatrotate(hinge_dir, transform->orientation);
				// �O�ςɂ���Č��Ă�bit��ς���
				if(vector3_dot(rot_axis, roted_hinge_dir) > 0)
					this_stage->tags |= left_tag;
				else
					this_stage->tags |= right_tag;


			};

		};

#pragma optimize("", on)


	}
}

