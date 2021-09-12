#pragma once

// �{�^�� ������stage_base�̕ϐ��̎w���bit��1�ɂȂ�

#include "../Adollib/Scripts/Object/component.h"
#include "../Adollib/Scripts/Physics/collider.h"

#include "stage_base.h"
#include "stageparts_tags.h"

namespace Adollib{

	namespace Stage_parts {
		// ��{�I��1��bit�𕡐��̃{�^���ŊǗ����Ȃ��B �{�^����uppdate��bit��0/1��؂�ւ��邽��
		// (component���g��Ȃ����bottanupdate���ɍs�����Ƃł��̖��͉������邪 32����΂Ƃ肠�������肻����������u)

		class Bottan : public Component {
			Stage_base* tihs_stage; //����parts�����݂��Ă���stage
			Collider* this_coll; //���g��collider

			Stageparts_tagbit flag = Stageparts_tags::None; //���g�̊Ǘ�����bit

		public:
			// ���t���[���Ă΂��X�V����
			void update() override {
				//player���������̂ɐG��Ă�����bit�𗧂Ă�
				if (this_coll->concoll_enter(Collider_tags::Human | Collider_tags::Kinematic_Stage)) {
					tihs_stage->tags |= flag;
				}
				else tihs_stage->tags &= ~flag;

			};

		};


	}
}

