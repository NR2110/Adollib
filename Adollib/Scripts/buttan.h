#pragma once

// �{�^�� ������stage_base�̕ϐ��̎w���bit��1�ɂȂ�

#include "stageparts_base.h"

namespace Adollib{

	namespace Stage_parts {

		class Buttan : public Stageparts_base {

			Stageparts_tagbit flag = Stageparts_tags::None; //���g�̊Ǘ�����bit

		public:

			// addComponent���ꂽ�Ƃ��ɌĂ΂��
			void awake() override {};

			// ���t���[���Ă΂��X�V����
			void update() override {
				//player���������̂ɐG��Ă�����bit�𗧂Ă�
				if (this_coll->concoll_enter(Collider_tags::Human | Collider_tags::Kinematic_Stage)) {
					this_stage->tags |= flag;
				}
				else this_stage->tags &= ~flag;

			};

		};


	}
}

