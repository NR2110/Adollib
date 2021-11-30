#pragma once

#include "stageparts_base.h"
#include "../Adollib/Scripts/Main/Adollib.h"

#include "stage_types.h"

namespace Adollib {
	class Stage_manager;

	namespace Stage_parts {

		// �w���tag��1�ł���Γ����M�~�b�N
		class Area_goal : public Stageparts_base {
		public:
			Stage_types next_stage = Stage_types::none;

		private:
			Stage_manager* stage_manager = nullptr; //stage_manager�ւ̃|�C���^


		public:

			// addComponent���ꂽ�Ƃ��ɌĂ΂��
			void awake() override;

			// ���t���[���Ă΂��X�V����
			void update() override;

		};


	}
}

